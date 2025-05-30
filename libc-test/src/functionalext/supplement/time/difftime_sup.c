/**
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdlib.h>
#include <time.h>
#include "functionalext.h"

struct difftime_struct {
    const char *tz;
    const double result;
};

static const struct difftime_struct gResultData[] = {
    {"Pacific/Rarotonga", 10.000000},
    {"Pacific/Wake", 10.000000},
    {"Pacific/Tarawa", 10.000000},
    {"Pacific/Fakaofo", 10.000000},
    {"Pacific/Apia", 10.000000},
    {"Pacific/Kwajalein", 10.000000},
    {"Pacific/Tahiti", 10.000000},
    {"Pacific/Easter", 10.000000},
    {"Pacific/Tongatapu", 10.000000},
    {"Pacific/Guadalcanal", 10.000000},
    {"Pacific/Chuuk", 10.000000},
    {"Pacific/Pitcairn", 10.000000},
    {"Pacific/Pohnpei", 10.000000},
    {"Pacific/Fiji", 10.000000},
    {"Pacific/Majuro", 10.000000},
    {"Pacific/Kiritimati", 10.000000},
    {"Pacific/Port_Moresby", 10.000000},
    {"Pacific/Galapagos", 10.000000},
    {"Pacific/Saipan", 10.000000},
    {"Pacific/Marquesas", 10.000000},
    {"Pacific/Wallis", 10.000000},
    {"Pacific/Palau", 10.000000},
    {"Pacific/Bougainville", 10.000000},
    {"Pacific/Honolulu", 10.000000},
    {"Pacific/Noumea", 10.000000},
    {"Pacific/Pago_Pago", 10.000000},
    {"Pacific/Guam", 10.000000},
    {"Pacific/Auckland", 10.000000},
    {"Pacific/Norfolk", 10.000000},
    {"Pacific/Kosrae", 10.000000},
    {"Pacific/Nauru", 10.000000},
    {"Pacific/Chatham", 10.000000},
    {"Pacific/Efate", 10.000000},
    {"Pacific/Midway", 10.000000},
    {"Pacific/Niue", 10.000000},
    {"Pacific/Enderbury", 10.000000},
    {"Pacific/Funafuti", 10.000000},
    {"Pacific/Gambier", 10.000000},
    {"EET", 10.000000},
    {"MST", 10.000000},
    {"Europe/Dublin", 10.000000},
    {"Europe/Minsk", 10.000000},
    {"Europe/Budapest", 10.000000},
    {"Europe/Riga", 10.000000},
    {"Europe/Jersey", 10.000000},
    {"Europe/London", 10.000000},
    {"Europe/Istanbul", 10.000000},
    {"Europe/Moscow", 10.000000},
    {"Europe/Chisinau", 10.000000},
    {"Europe/Andorra", 10.000000},
    {"Europe/Vaduz", 10.000000},
    {"Europe/Zaporozhye", 10.000000},
    {"Europe/San_Marino", 10.000000},
    {"Europe/Monaco", 10.000000},
    {"Europe/Ulyanovsk", 10.000000},
    {"Europe/Isle_of_Man", 10.000000},
    {"Europe/Tirane", 10.000000},
    {"Europe/Vatican", 10.000000},
    {"Europe/Belgrade", 10.000000},
    {"Europe/Paris", 10.000000},
    {"Europe/Nicosia", 10.000000},
    {"Europe/Athens", 10.000000},
    {"Europe/Malta", 10.000000},
    {"Europe/Astrakhan", 10.000000},
    {"Europe/Bratislava", 10.000000},
    {"Europe/Uzhgorod", 10.000000},
    {"Europe/Zurich", 10.000000},
    {"Europe/Brussels", 10.000000},
    {"Europe/Sofia", 10.000000},
    {"Europe/Kiev", 10.000000},
    {"Europe/Guernsey", 10.000000},
    {"Europe/Busingen", 10.000000},
    {"Europe/Copenhagen", 10.000000},
    {"Europe/Skopje", 10.000000},
    {"Europe/Luxembourg", 10.000000},
    {"Europe/Lisbon", 10.000000},
    {"Europe/Saratov", 10.000000},
    {"Europe/Zagreb", 10.000000},
    {"Europe/Simferopol", 10.000000},
    {"Europe/Amsterdam", 10.000000},
    {"Europe/Oslo", 10.000000},
    {"Europe/Samara", 10.000000},
    {"Europe/Warsaw", 10.000000},
    {"Europe/Prague", 10.000000},
    {"Europe/Vienna", 10.000000},
    {"Europe/Mariehamn", 10.000000},
    {"Europe/Gibraltar", 10.000000},
    {"Europe/Stockholm", 10.000000},
    {"Europe/Tallinn", 10.000000},
    {"Europe/Kaliningrad", 10.000000},
    {"Europe/Podgorica", 10.000000},
    {"Europe/Kirov", 10.000000},
    {"Europe/Helsinki", 10.000000},
    {"Europe/Sarajevo", 10.000000},
    {"Europe/Rome", 10.000000},
    {"Europe/Bucharest", 10.000000},
    {"Europe/Berlin", 10.000000},
    {"Europe/Volgograd", 10.000000},
    {"Europe/Ljubljana", 10.000000},
    {"Europe/Vilnius", 10.000000},
    {"Europe/Madrid", 10.000000},
    {"Antarctica/Casey", 10.000000},
    {"Antarctica/Macquarie", 10.000000},
    {"Antarctica/Rothera", 10.000000},
    {"Antarctica/Vostok", 10.000000},
    {"Antarctica/Davis", 10.000000},
    {"Antarctica/Troll", 10.000000},
    {"Antarctica/Mawson", 10.000000},
    {"Antarctica/McMurdo", 10.000000},
    {"Antarctica/DumontDUrville", 10.000000},
    {"Antarctica/Palmer", 10.000000},
    {"Antarctica/Syowa", 10.000000},
    {"WET", 10.000000},
    {"CST6CDT", 10.000000},
    {"MET", 10.000000},
    {"Arctic/Longyearbyen", 10.000000},
    {"Atlantic/Faroe", 10.000000},
    {"Atlantic/Madeira", 10.000000},
    {"Atlantic/Canary", 10.000000},
    {"Atlantic/Reykjavik", 10.000000},
    {"Atlantic/St_Helena", 10.000000},
    {"Atlantic/South_Georgia", 10.000000},
    {"Atlantic/Azores", 10.000000},
    {"Atlantic/Bermuda", 10.000000},
    {"Atlantic/Stanley", 10.000000},
    {"Atlantic/Cape_Verde", 10.000000},
    {"Africa/Niamey", 10.000000},
    {"Africa/Algiers", 10.000000},
    {"Africa/Maputo", 10.000000},
    {"Africa/Johannesburg", 10.000000},
    {"Africa/Addis_Ababa", 10.000000},
    {"Africa/Malabo", 10.000000},
    {"Africa/Cairo", 10.000000},
    {"Africa/Bissau", 10.000000},
    {"Africa/Ndjamena", 10.000000},
    {"Africa/Tunis", 10.000000},
    {"Africa/Harare", 10.000000},
    {"Africa/El_Aaiun", 10.000000},
    {"Africa/Kampala", 10.000000},
    {"Africa/Libreville", 10.000000},
    {"Africa/Blantyre", 10.000000},
    {"Africa/Brazzaville", 10.000000},
    {"Africa/Bangui", 10.000000},
    {"Africa/Luanda", 10.000000},
    {"Africa/Freetown", 10.000000},
    {"Africa/Casablanca", 10.000000},
    {"Africa/Conakry", 10.000000},
    {"Africa/Kinshasa", 10.000000},
    {"Africa/Dakar", 10.000000},
    {"Africa/Ouagadougou", 10.000000},
    {"Africa/Nouakchott", 10.000000},
    {"Africa/Monrovia", 10.000000},
    {"Africa/Kigali", 10.000000},
    {"Africa/Windhoek", 10.000000},
    {"Africa/Juba", 10.000000},
    {"Africa/Sao_Tome", 10.000000},
    {"Africa/Banjul", 10.000000},
    {"Africa/Mbabane", 10.000000},
    {"Africa/Asmara", 10.000000},
    {"Africa/Djibouti", 10.000000},
    {"Africa/Tripoli", 10.000000},
    {"Africa/Ceuta", 10.000000},
    {"Africa/Khartoum", 10.000000},
    {"Africa/Bamako", 10.000000},
    {"Africa/Porto-Novo", 10.000000},
    {"Africa/Mogadishu", 10.000000},
    {"Africa/Abidjan", 10.000000},
    {"Africa/Lusaka", 10.000000},
    {"Africa/Lagos", 10.000000},
    {"Africa/Lubumbashi", 10.000000},
    {"Africa/Nairobi", 10.000000},
    {"Africa/Accra", 10.000000},
    {"Africa/Maseru", 10.000000},
    {"Africa/Lome", 10.000000},
    {"Africa/Douala", 10.000000},
    {"Africa/Bujumbura", 10.000000},
    {"Africa/Dar_es_Salaam", 10.000000},
    {"Africa/Gaborone", 10.000000},
    {"Australia/Adelaide", 10.000000},
    {"Australia/Broken_Hill", 10.000000},
    {"Australia/Perth", 10.000000},
    {"Australia/Darwin", 10.000000},
    {"Australia/Lord_Howe", 10.000000},
    {"Australia/Hobart", 10.000000},
    {"Australia/Brisbane", 10.000000},
    {"Australia/Sydney", 10.000000},
    {"Australia/Lindeman", 10.000000},
    {"Australia/Melbourne", 10.000000},
    {"Australia/Eucla", 10.000000},
    {"America/Grand_Turk", 10.000000},
    {"America/St_Thomas", 10.000000},
    {"America/Havana", 10.000000},
    {"America/Thunder_Bay", 10.000000},
    {"America/Inuvik", 10.000000},
    {"America/Winnipeg", 10.000000},
    {"America/Maceio", 10.000000},
    {"America/Chihuahua", 10.000000},
    {"America/North_Dakota/Beulah", 10.000000},
    {"America/North_Dakota/Center", 10.000000},
    {"America/North_Dakota/New_Salem", 10.000000},
    {"America/Campo_Grande", 10.000000},
    {"America/Scoresbysund", 10.000000},
    {"America/Tijuana", 10.000000},
    {"America/Bahia", 10.000000},
    {"America/St_Vincent", 10.000000},
    {"America/Creston", 10.000000},
    {"America/Dawson", 10.000000},
    {"America/Tegucigalpa", 10.000000},
    {"America/Lima", 10.000000},
    {"America/Mexico_City", 10.000000},
    {"America/Dominica", 10.000000},
    {"America/Atikokan", 10.000000},
    {"America/Port-au-Prince", 10.000000},
    {"America/Yakutat", 10.000000},
    {"America/St_Kitts", 10.000000},
    {"America/Guatemala", 10.000000},
    {"America/St_Barthelemy", 10.000000},
    {"America/Vancouver", 10.000000},
    {"America/Jamaica", 10.000000},
    {"America/Whitehorse", 10.000000},
    {"America/Asuncion", 10.000000},
    {"America/Toronto", 10.000000},
    {"America/Hermosillo", 10.000000},
    {"America/Denver", 10.000000},
    {"America/Mazatlan", 10.000000},
    {"America/Iqaluit", 10.000000},
    {"America/Nuuk", 10.000000},
    {"America/Cayenne", 10.000000},
    {"America/Santarem", 10.000000},
    {"America/Noronha", 10.000000},
    {"America/Ojinaga", 10.000000},
    {"America/Fort_Nelson", 10.000000},
    {"America/Aruba", 10.000000},
    {"America/Indiana/Vevay", 10.000000},
    {"America/Indiana/Marengo", 10.000000},
    {"America/Indiana/Indianapolis", 10.000000},
    {"America/Indiana/Tell_City", 10.000000},
    {"America/Indiana/Petersburg", 10.000000},
    {"America/Indiana/Winamac", 10.000000},
    {"America/Indiana/Knox", 10.000000},
    {"America/Indiana/Vincennes", 10.000000},
    {"America/Cayman", 10.000000},
    {"America/Sao_Paulo", 10.000000},
    {"America/Curacao", 10.000000},
    {"America/Anchorage", 10.000000},
    {"America/Guyana", 10.000000},
    {"America/Rankin_Inlet", 10.000000},
    {"America/Antigua", 10.000000},
    {"America/Juneau", 10.000000},
    {"America/Merida", 10.000000},
    {"America/Punta_Arenas", 10.000000},
    {"America/Moncton", 10.000000},
    {"America/Fortaleza", 10.000000},
    {"America/Resolute", 10.000000},
    {"America/Nassau", 10.000000},
    {"America/Edmonton", 10.000000},
    {"America/Regina", 10.000000},
    {"America/Halifax", 10.000000},
    {"America/Danmarkshavn", 10.000000},
    {"America/Metlakatla", 10.000000},
    {"America/Guadeloupe", 10.000000},
    {"America/Caracas", 10.000000},
    {"America/Detroit", 10.000000},
    {"America/Matamoros", 10.000000},
    {"America/Glace_Bay", 10.000000},
    {"America/Phoenix", 10.000000},
    {"America/La_Paz", 10.000000},
    {"America/Swift_Current", 10.000000},
    {"America/Nipigon", 10.000000},
    {"America/Cancun", 10.000000},
    {"America/Port_of_Spain", 10.000000},
    {"America/Kralendijk", 10.000000},
    {"America/Grenada", 10.000000},
    {"America/Belem", 10.000000},
    {"America/Managua", 10.000000},
    {"America/Adak", 10.000000},
    {"America/St_Lucia", 10.000000},
    {"America/Rio_Branco", 10.000000},
    {"America/Santiago", 10.000000},
    {"America/Puerto_Rico", 10.000000},
    {"America/Miquelon", 10.000000},
    {"America/Cuiaba", 10.000000},
    {"America/Martinique", 10.000000},
    {"America/Argentina/Buenos_Aires", 10.000000},
    {"America/Argentina/San_Luis", 10.000000},
    {"America/Argentina/Catamarca", 10.000000},
    {"America/Argentina/Jujuy", 10.000000},
    {"America/Argentina/Salta", 10.000000},
    {"America/Argentina/Ushuaia", 10.000000},
    {"America/Argentina/Mendoza", 10.000000},
    {"America/Argentina/Rio_Gallegos", 10.000000},
    {"America/Argentina/Tucuman", 10.000000},
    {"America/Argentina/San_Juan", 10.000000},
    {"America/Argentina/Cordoba", 10.000000},
    {"America/Argentina/La_Rioja", 10.000000},
    {"America/Barbados", 10.000000},
    {"America/Monterrey", 10.000000},
    {"America/Thule", 10.000000},
    {"America/Montserrat", 10.000000},
    {"America/Guayaquil", 10.000000},
    {"America/St_Johns", 10.000000},
    {"America/Boise", 10.000000},
    {"America/Manaus", 10.000000},
    {"America/Nome", 10.000000},
    {"America/Goose_Bay", 10.000000},
    {"America/Belize", 10.000000},
    {"America/Sitka", 10.000000},
    {"America/Dawson_Creek", 10.000000},
    {"America/Recife", 10.000000},
    {"America/Kentucky/Louisville", 10.000000},
    {"America/Kentucky/Monticello", 10.000000},
    {"America/New_York", 10.000000},
    {"America/Tortola", 10.000000},
    {"America/Chicago", 10.000000},
    {"America/Pangnirtung", 10.000000},
    {"America/Boa_Vista", 10.000000},
    {"America/Bogota", 10.000000},
    {"America/Rainy_River", 10.000000},
    {"America/El_Salvador", 10.000000},
    {"America/Bahia_Banderas", 10.000000},
    {"America/Yellowknife", 10.000000},
    {"America/Montevideo", 10.000000},
    {"America/Cambridge_Bay", 10.000000},
    {"America/Panama", 10.000000},
    {"America/Santo_Domingo", 10.000000},
    {"America/Anguilla", 10.000000},
    {"America/Paramaribo", 10.000000},
    {"America/Marigot", 10.000000},
    {"America/Menominee", 10.000000},
    {"America/Blanc-Sablon", 10.000000},
    {"America/Costa_Rica", 10.000000},
    {"America/Los_Angeles", 10.000000},
    {"America/Lower_Princes", 10.000000},
    {"America/Eirunepe", 10.000000},
    {"America/Araguaina", 10.000000},
    {"America/Porto_Velho", 10.000000},
    {"EST", 10.000000},
    {"CET", 10.000000},
    {"MST7MDT", 10.000000},
    {"Indian/Chagos", 10.000000},
    {"Indian/Kerguelen", 10.000000},
    {"Indian/Reunion", 10.000000},
    {"Indian/Antananarivo", 10.000000},
    {"Indian/Cocos", 10.000000},
    {"Indian/Mauritius", 10.000000},
    {"Indian/Christmas", 10.000000},
    {"Indian/Maldives", 10.000000},
    {"Indian/Comoro", 10.000000},
    {"Indian/Mahe", 10.000000},
    {"Indian/Mayotte", 10.000000},
    {"Asia/Novokuznetsk", 10.000000},
    {"Asia/Magadan", 10.000000},
    {"Asia/Yangon", 10.000000},
    {"Asia/Samarkand", 10.000000},
    {"Asia/Tehran", 10.000000},
    {"Asia/Dili", 10.000000},
    {"Asia/Yerevan", 10.000000},
    {"Asia/Qatar", 10.000000},
    {"Asia/Chita", 10.000000},
    {"Asia/Makassar", 10.000000},
    {"Asia/Hovd", 10.000000},
    {"Asia/Istanbul", 10.000000},
    {"Asia/Irkutsk", 10.000000},
    {"Asia/Pontianak", 10.000000},
    {"Asia/Qyzylorda", 10.000000},
    {"Asia/Srednekolymsk", 10.000000},
    {"Asia/Aden", 10.000000},
    {"Asia/Kabul", 10.000000},
    {"Asia/Macau", 10.000000},
    {"Asia/Riyadh", 10.000000},
    {"Asia/Taipei", 10.000000},
    {"Asia/Manila", 10.000000},
    {"Asia/Aqtau", 10.000000},
    {"Asia/Thimphu", 10.000000},
    {"Asia/Shanghai", 10.000000},
    {"Asia/Choibalsan", 10.000000},
    {"Asia/Nicosia", 10.000000},
    {"Asia/Dubai", 10.000000},
    {"Asia/Baku", 10.000000},
    {"Asia/Kuching", 10.000000},
    {"Asia/Gaza", 10.000000},
    {"Asia/Jayapura", 10.000000},
    {"Asia/Tokyo", 10.000000},
    {"Asia/Muscat", 10.000000},
    {"Asia/Kolkata", 10.000000},
    {"Asia/Ho_Chi_Minh", 10.000000},
    {"Asia/Tashkent", 10.000000},
    {"Asia/Bangkok", 10.000000},
    {"Asia/Karachi", 10.000000},
    {"Asia/Krasnoyarsk", 10.000000},
    {"Asia/Yekaterinburg", 10.000000},
    {"Asia/Famagusta", 10.000000},
    {"Asia/Ulaanbaatar", 10.000000},
    {"Asia/Bahrain", 10.000000},
    {"Asia/Barnaul", 10.000000},
    {"Asia/Jerusalem", 10.000000},
    {"Asia/Qostanay", 10.000000},
    {"Asia/Ust-Nera", 10.000000},
    {"Asia/Tomsk", 10.000000},
    {"Asia/Kuala_Lumpur", 10.000000},
    {"Asia/Brunei", 10.000000},
    {"Asia/Aqtobe", 10.000000},
    {"Asia/Vladivostok", 10.000000},
    {"Asia/Damascus", 10.000000},
    {"Asia/Omsk", 10.000000},
    {"Asia/Almaty", 10.000000},
    {"Asia/Dhaka", 10.000000},
    {"Asia/Tbilisi", 10.000000},
    {"Asia/Baghdad", 10.000000},
    {"Asia/Phnom_Penh", 10.000000},
    {"Asia/Kamchatka", 10.000000},
    {"Asia/Ashgabat", 10.000000},
    {"Asia/Sakhalin", 10.000000},
    {"Asia/Novosibirsk", 10.000000},
    {"Asia/Kuwait", 10.000000},
    {"Asia/Hong_Kong", 10.000000},
    {"Asia/Amman", 10.000000},
    {"Asia/Beirut", 10.000000},
    {"Asia/Yakutsk", 10.000000},
    {"Asia/Colombo", 10.000000},
    {"Asia/Oral", 10.000000},
    {"Asia/Kathmandu", 10.000000},
    {"Asia/Bishkek", 10.000000},
    {"Asia/Hebron", 10.000000},
    {"Asia/Singapore", 10.000000},
    {"Asia/Atyrau", 10.000000},
    {"Asia/Khandyga", 10.000000},
    {"Asia/Vientiane", 10.000000},
    {"Asia/Jakarta", 10.000000},
    {"Asia/Dushanbe", 10.000000},
    {"Asia/Seoul", 10.000000},
    {"Asia/Pyongyang", 10.000000},
    {"Asia/Urumqi", 10.000000},
    {"Asia/Anadyr", 10.000000},
    {"PST8PDT", 10.000000},
    {"HST", 10.000000},
    {"EST5EDT", 10.000000},
};

/**
 * @tc.name      : difftime_0100
 * @tc.desc      : according to different time zones, calculate the time difference between two moments
 * @tc.level     : Level 0
 */
void difftime_0100(void)
{
    time_t timeStart = 20000;
    time_t timeEnd = 20010;
    for (int32_t i = 0; i < (int32_t)(sizeof(gResultData) / sizeof(gResultData[0])); i++) {
        const char *tz = gResultData[i].tz;
        const char *handlerChar;
#ifdef TIME_ZONE_SUB_TAG
        char *str = strrchr(tz, TIME_ZONE_SUB_TAG);
        if (str) {
            handlerChar = ++str;
        } else {
            handlerChar = tz;
        }
#else
        handlerChar = tz;
#endif
        setenv("TZ", handlerChar, 1);
        tzset();
        double returnVal;
        returnVal = difftime(timeEnd, timeStart);
        EXPECT_TRUE(
            "difftime_0100", abs(gResultData[i].result - returnVal) >= 0 && abs(gResultData[i].result - returnVal) < 1);
    }
}

int main(void)
{
    difftime_0100();
    return t_status;
}