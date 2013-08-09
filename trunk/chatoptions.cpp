/*
 * This file is part of THT.
 *
 * THT is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * THT is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with THT.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QIcon>

#include "chatsettings.h"
#include "chatoptions.h"
#include "settings.h"
#include "ui_chatoptions.h"

ChatOptions::ChatOptions(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChatOptions)
{
    ui->setupUi(this);

    load();

    ui->labelIcon->setPixmap(QIcon(":/images/chat.ico").pixmap(ui->labelIcon->size()));
}

ChatOptions::~ChatOptions()
{
    delete ui;
}

void ChatOptions::saveSettings()
{
    SETTINGS_SET_BOOL(SETTING_CHAT_AUTO_LOGIN, ui->checkAutoLogin->isChecked(), Settings::NoSync);
    SETTINGS_SET_BOOL(SETTING_CHAT_SAVE_ROOMS, ui->checkSaveRooms->isChecked(), Settings::NoSync);
    SETTINGS_SET_BOOL(SETTING_CHAT_AUTO_LOGIN_TO_ROOMS, ui->checkAutoLoginToRooms->isChecked(), Settings::NoSync);
    SETTINGS_SET_BOOL(SETTING_CHAT_RESTORE_AT_STARTUP, ui->checkRestoreChat->isChecked());
}

void ChatOptions::load()
{
    ui->checkAutoLogin->setChecked(SETTINGS_GET_BOOL(SETTING_CHAT_AUTO_LOGIN));
    ui->checkSaveRooms->setChecked(SETTINGS_GET_BOOL(SETTING_CHAT_SAVE_ROOMS));
    ui->checkAutoLoginToRooms->setChecked(SETTINGS_GET_BOOL(SETTING_CHAT_AUTO_LOGIN_TO_ROOMS));
    ui->checkRestoreChat->setChecked(SETTINGS_GET_BOOL(SETTING_CHAT_RESTORE_AT_STARTUP));
}
