#ifndef PEARLCALCULATOR_GUI_I18N_H
#define PEARLCALCULATOR_GUI_I18N_H

namespace pearl::gui
{
    enum class Language
    {
        English      = 0,
        ChineseSimp  = 1
    };

    void setLanguage(Language lang);
    Language currentLanguage();

    // 通过 key 取本地化字符串；找不到时返回 key 本身。
    const char* t(const char* key);
}

#endif
