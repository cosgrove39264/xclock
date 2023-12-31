//
// Created by marcel on 14.10.23.
//

#include <WiFiClientSecure.h>
#include "Bot.h"
#include "WLan.h"
#include "SetupMode.h"
#include "AsyncTelegram2.h"
#include "bot_config.h"
#include "Dice.h"
#include "ArduinoNvs.h"


bool Bot::started = false;
bool Bot::enabled = false;

void Bot::start_bot() {
    if (started) {
        return;
    }
    if (!WLan::connected) {
        return;
    }
    if (SetupMode::ota_started) {
        return;
    }
    Bot::enabled = NVS.getInt("bot_enabled") > 2;

    if (!Bot::enabled) {
        return;
    }
    Serial.println("Aktiviere Bot");

    TaskHandle_t taskHandle;
    xTaskCreate(Bot::task,
                "Bot",
                4096,
                nullptr,
                10,
                &taskHandle);
}

void Bot::task(void *pvParameters) {
    if (!WLan::connected) {
        Bot::started = false;

        vTaskDelete(NULL);
        return;
    }
    if (SetupMode::ota_started) {
        Bot::started = false;
        vTaskDelete(NULL);
        return;
    }

    Bot::started = true;

    WiFiClientSecure client;
    AsyncTelegram2 myBot(client);

    client.setCACert(telegram_cert);
    myBot.setUpdateTime(2000);
    myBot.setTelegramToken(bot_token);
    if (!myBot.begin()) {
        Serial.println("Bot nicht gestartet");
        Bot::started = false;
        vTaskDelete(NULL);
        return;
    }
    bool start_ota = false;

    for (;;) {
        if (SetupMode::ota_started) {
            break;
        }
        if (!WLan::connected) {
            break;
        }
        TBMessage msg;
        if (myBot.getNewMessage(msg)) {
            switch (msg.messageType) {
                case MessageDocument : {

                    myBot.sendMessage(msg, "nicht möglich");

                }
                    break;
                case MessageReply: {
                    myBot.sendMessage(msg, "You have entered wrong term");
                }
                    break;

                default: {

                    if (     msg.text.equalsIgnoreCase("/version")) {
                        String fw = "XClock Version: " + String(SetupMode::firmware_version);
                        myBot.sendMessage(msg, fw);
                    } else if (msg.text.equalsIgnoreCase("/dice")) {
                        Dice::stop = true;
                        while (Dice::running) {
                            vTaskDelay(5 / portTICK_PERIOD_MS);
                        }
                        Dice::run(15);
                        myBot.sendMessage(msg, "Die Würfel sind gefallen");
                    } else if (msg.text.equalsIgnoreCase("/upgrade")) {

                        if (millis() < 30000) {
                            myBot.sendMessage(msg, "für ein Firmware-Update ist es noch zu früh");
                        } else {
                            start_ota = true;
                            myBot.sendMessage(msg, "Firmware-Update wird gestartet");
                        }
                    } else if (msg.text.equalsIgnoreCase("/zeit")) {
                        time_t now;
                        struct tm timeinfo{};
                        char buffer[80];

                        time(&now);
                        localtime_r(&now, &timeinfo);
                        strftime(buffer, 80, "%A, %B %d %Y %H:%M:%S", &timeinfo);
                        myBot.sendMessage(msg, "Aktuelle Zeit: " + String(buffer));

                    } else if (msg.text.equalsIgnoreCase("/help")) {
                        myBot.sendMessage(msg, "Verfügbare Optionen");
                        myBot.sendMessage(msg, "/dice - würfeln");
                        myBot.sendMessage(msg, "/upgrade - Firmware-Update");
                        myBot.sendMessage(msg, "/zeit - aktuelle Zeit");
                        myBot.sendMessage(msg, "/version - XClock Version");
                        myBot.sendMessage(msg, "/disable - deaktiviert den Bot");
                    } else {
                        Bot::send_welcome(myBot, msg.chatId);
                    }
                    break;
                }
            }
        } else {
            vTaskDelay(50 / portTICK_PERIOD_MS);
            if (start_ota) {
                break;
            }
        }
    }
    if (start_ota) {
        SetupMode::start_http_ota();
    }
    vTaskDelete(NULL);
}

void Bot::send_welcome(AsyncTelegram2 bot, uint64_t chat_id) {
    char welcome_msg[128];
    snprintf(welcome_msg, 128, "%s\n\n/help alle verfügbaren Optionen", bot.getBotName());
    bot.sendTo(chat_id, welcome_msg);

}

void Bot::process_button_click(uint8_t button, uint8_t type) {
    if (button == 1) {
        if (type == 5) {
            if (Bot::enabled) {
                NVS.setInt("bot_enabled", 1, true);
                SetupMode::reboot();
            } else {
                NVS.setInt("bot_enabled", 2, true);

            }
        }
    }
}
