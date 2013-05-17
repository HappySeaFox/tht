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

#include <QString>

#include "settings.h"
#include "options.h"

#include "ui_options.h"

Options::Options(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Options),
    m_startIndex(-1)
{
    ui->setupUi(this);
    ui->labelRestart->hide();

    load();
}

Options::~Options()
{
    delete ui;
}

void Options::load()
{
    for(int i = 0;i < Settings::instance()->maximumNumberOfLists();i++)
        ui->comboNumberOfLists->addItem(QString::number(i+1));

    ui->comboNumberOfLists->setCurrentIndex(Settings::instance()->numberOfLists() - 1);
    ui->checkOnTop->setChecked(SETTINGS_GET_BOOL(SETTING_ONTOP));
    ui->checkTray->setChecked(SETTINGS_GET_BOOL(SETTING_HIDE_TO_TRAY));
    ui->checkRestoreIndustries->setChecked(SETTINGS_GET_BOOL(SETTING_RESTORE_NEIGHBORS_AT_STARTUP));
    ui->checkSave->setChecked(SETTINGS_GET_BOOL(SETTING_SAVE_GEOMETRY));
    ui->checkSaveTickers->setChecked(SETTINGS_GET_BOOL(SETTING_SAVE_TICKERS));
    ui->checkAllowDups->setChecked(SETTINGS_GET_BOOL(SETTING_ALLOW_DUPLICATES));
    ui->checkMini->setChecked(SETTINGS_GET_BOOL(SETTING_MINI_TICKER_ENTRY));
    ui->checkHeader->setChecked(SETTINGS_GET_BOOL(SETTING_LIST_HEADER));
    ui->checkComments->setChecked(SETTINGS_GET_BOOL(SETTING_SHOW_COMMENTS));
    ui->checkRestoreLP->setChecked(SETTINGS_GET_BOOL(SETTING_RESTORE_LINKS_AT_STARTUP));

    const QMap<QString, QString> tsmap = Settings::instance()->translations();
    QString ts = SETTINGS_GET_STRING(SETTING_TRANSLATION);
    QMap<QString, QString>::const_iterator itEnd = tsmap.end();

    for(QMap<QString, QString>::const_iterator it = tsmap.begin();it != itEnd;++it)
    {
        ui->comboLang->addItem(it.key(), it.value());

        if(it.value() == ts)
        {
            m_startIndex = ui->comboLang->count() - 1;
            ui->comboLang->setCurrentIndex(m_startIndex);
        }
    }

    if(m_startIndex < 0)
        m_startIndex = 0;
}

void Options::slotLanguageChanged(int index)
{
    ui->labelRestart->setVisible(index != m_startIndex);
}

void Options::saveSettings() const
{
    Settings::instance()->setNumberOfLists(ui->comboNumberOfLists->currentIndex()+1, Settings::NoSync);
    SETTINGS_SET_STRING(SETTING_TRANSLATION, ui->comboLang->itemData(ui->comboLang->currentIndex()).toString(), Settings::NoSync);
    SETTINGS_SET_BOOL(SETTING_ONTOP, ui->checkOnTop->isChecked(), Settings::NoSync);
    SETTINGS_SET_BOOL(SETTING_HIDE_TO_TRAY, ui->checkTray->isChecked(), Settings::NoSync);
    SETTINGS_SET_BOOL(SETTING_RESTORE_NEIGHBORS_AT_STARTUP, ui->checkRestoreIndustries->isChecked(), Settings::NoSync);
    SETTINGS_SET_BOOL(SETTING_SAVE_GEOMETRY, ui->checkSave->isChecked(), Settings::NoSync);
    SETTINGS_SET_BOOL(SETTING_SAVE_TICKERS, ui->checkSaveTickers->isChecked(), Settings::NoSync);
    SETTINGS_SET_BOOL(SETTING_ALLOW_DUPLICATES, ui->checkAllowDups->isChecked(), Settings::NoSync);
    SETTINGS_SET_BOOL(SETTING_MINI_TICKER_ENTRY, ui->checkMini->isChecked(), Settings::NoSync);
    SETTINGS_SET_BOOL(SETTING_LIST_HEADER, ui->checkHeader->isChecked(), Settings::NoSync);
    SETTINGS_SET_BOOL(SETTING_SHOW_COMMENTS, ui->checkComments->isChecked(), Settings::NoSync);
    SETTINGS_SET_BOOL(SETTING_RESTORE_LINKS_AT_STARTUP, ui->checkComments->isChecked()); // also sync
}
