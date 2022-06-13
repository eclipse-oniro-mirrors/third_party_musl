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

struct strftime_struct {
    const char *tz;
    const char *result;
};

static const struct strftime_struct gResultData[] = {
    {"Pacific/Rarotonga", "Sat Jul 30 00:40:14 2022"},
    {"Pacific/Wake", "Sat Jul 30 22:40:14 2022"},
    {"Pacific/Tarawa", "Sat Jul 30 22:40:14 2022"},
    {"Pacific/Fakaofo", "Sat Jul 30 23:40:14 2022"},
    {"Pacific/Apia", "Sat Jul 30 23:40:14 2022"},
    {"Pacific/Kwajalein", "Sat Jul 30 22:40:14 2022"},
    {"Pacific/Tahiti", "Sat Jul 30 00:40:14 2022"},
    {"Pacific/Easter", "Sat Jul 30 04:40:14 2022"},
    {"Pacific/Tongatapu", "Sat Jul 30 23:40:14 2022"},
    {"Pacific/Guadalcanal", "Sat Jul 30 21:40:14 2022"},
    {"Pacific/Chuuk", "Sat Jul 30 20:40:14 2022"},
    {"Pacific/Pitcairn", "Sat Jul 30 02:40:14 2022"},
    {"Pacific/Pohnpei", "Sat Jul 30 21:40:14 2022"},
    {"Pacific/Fiji", "Sat Jul 30 22:40:14 2022"},
    {"Pacific/Majuro", "Sat Jul 30 22:40:14 2022"},
    {"Pacific/Kiritimati", "Sun Jul 31 00:40:14 2022"},
    {"Pacific/Port_Moresby", "Sat Jul 30 20:40:14 2022"},
    {"Pacific/Galapagos", "Sat Jul 30 04:40:14 2022"},
    {"Pacific/Saipan", "Sat Jul 30 20:40:14 2022"},
    {"Pacific/Marquesas", "Sat Jul 30 01:10:14 2022"},
    {"Pacific/Wallis", "Sat Jul 30 22:40:14 2022"},
    {"Pacific/Palau", "Sat Jul 30 19:40:14 2022"},
    {"Pacific/Bougainville", "Sat Jul 30 21:40:14 2022"},
    {"Pacific/Honolulu", "Sat Jul 30 00:40:14 2022"},
    {"Pacific/Noumea", "Sat Jul 30 21:40:14 2022"},
    {"Pacific/Pago_Pago", "Fri Jul 29 23:40:14 2022"},
    {"Pacific/Guam", "Sat Jul 30 20:40:14 2022"},
    {"Pacific/Auckland", "Sat Jul 30 22:40:14 2022"},
    {"Pacific/Norfolk", "Sat Jul 30 21:40:14 2022"},
    {"Pacific/Kosrae", "Sat Jul 30 21:40:14 2022"},
    {"Pacific/Nauru", "Sat Jul 30 22:40:14 2022"},
    {"Pacific/Chatham", "Sat Jul 30 23:25:14 2022"},
    {"Pacific/Efate", "Sat Jul 30 21:40:14 2022"},
    {"Pacific/Midway", "Fri Jul 29 23:40:14 2022"},
    {"Pacific/Niue", "Fri Jul 29 23:40:14 2022"},
    {"Pacific/Enderbury", "Sat Jul 30 23:40:14 2022"},
    {"Pacific/Funafuti", "Sat Jul 30 22:40:14 2022"},
    {"Pacific/Gambier", "Sat Jul 30 01:40:14 2022"},
    {"EET", "Sat Jul 30 13:40:14 2022"},
    {"MST", "Sat Jul 30 03:40:14 2022"},
    {"Europe/Dublin", "Sat Jul 30 11:40:14 2022"},
    {"Europe/Minsk", "Sat Jul 30 13:40:14 2022"},
    {"Europe/Budapest", "Sat Jul 30 12:40:14 2022"},
    {"Europe/Riga", "Sat Jul 30 13:40:14 2022"},
    {"Europe/Jersey", "Sat Jul 30 11:40:14 2022"},
    {"Europe/London", "Sat Jul 30 11:40:14 2022"},
    {"Europe/Istanbul", "Sat Jul 30 13:40:14 2022"},
    {"Europe/Moscow", "Sat Jul 30 13:40:14 2022"},
    {"Europe/Chisinau", "Sat Jul 30 13:40:14 2022"},
    {"Europe/Andorra", "Sat Jul 30 12:40:14 2022"},
    {"Europe/Vaduz", "Sat Jul 30 12:40:14 2022"},
    {"Europe/Zaporozhye", "Sat Jul 30 13:40:14 2022"},
    {"Europe/San_Marino", "Sat Jul 30 12:40:14 2022"},
    {"Europe/Monaco", "Sat Jul 30 12:40:14 2022"},
    {"Europe/Ulyanovsk", "Sat Jul 30 14:40:14 2022"},
    {"Europe/Isle_of_Man", "Sat Jul 30 11:40:14 2022"},
    {"Europe/Tirane", "Sat Jul 30 12:40:14 2022"},
    {"Europe/Vatican", "Sat Jul 30 12:40:14 2022"},
    {"Europe/Belgrade", "Sat Jul 30 12:40:14 2022"},
    {"Europe/Paris", "Sat Jul 30 12:40:14 2022"},
    {"Europe/Nicosia", "Sat Jul 30 13:40:14 2022"},
    {"Europe/Athens", "Sat Jul 30 13:40:14 2022"},
    {"Europe/Malta", "Sat Jul 30 12:40:14 2022"},
    {"Europe/Astrakhan", "Sat Jul 30 14:40:14 2022"},
    {"Europe/Bratislava", "Sat Jul 30 12:40:14 2022"},
    {"Europe/Uzhgorod", "Sat Jul 30 13:40:14 2022"},
    {"Europe/Zurich", "Sat Jul 30 12:40:14 2022"},
    {"Europe/Brussels", "Sat Jul 30 12:40:14 2022"},
    {"Europe/Sofia", "Sat Jul 30 13:40:14 2022"},
    {"Europe/Kiev", "Sat Jul 30 13:40:14 2022"},
    {"Europe/Guernsey", "Sat Jul 30 11:40:14 2022"},
    {"Europe/Busingen", "Sat Jul 30 12:40:14 2022"},
    {"Europe/Copenhagen", "Sat Jul 30 12:40:14 2022"},
    {"Europe/Skopje", "Sat Jul 30 12:40:14 2022"},
    {"Europe/Luxembourg", "Sat Jul 30 12:40:14 2022"},
    {"Europe/Lisbon", "Sat Jul 30 11:40:14 2022"},
    {"Europe/Saratov", "Sat Jul 30 14:40:14 2022"},
    {"Europe/Zagreb", "Sat Jul 30 12:40:14 2022"},
    {"Europe/Simferopol", "Sat Jul 30 13:40:14 2022"},
    {"Europe/Amsterdam", "Sat Jul 30 12:40:14 2022"},
    {"Europe/Oslo", "Sat Jul 30 12:40:14 2022"},
    {"Europe/Samara", "Sat Jul 30 14:40:14 2022"},
    {"Europe/Warsaw", "Sat Jul 30 12:40:14 2022"},
    {"Europe/Prague", "Sat Jul 30 12:40:14 2022"},
    {"Europe/Vienna", "Sat Jul 30 12:40:14 2022"},
    {"Europe/Mariehamn", "Sat Jul 30 13:40:14 2022"},
    {"Europe/Gibraltar", "Sat Jul 30 12:40:14 2022"},
    {"Europe/Stockholm", "Sat Jul 30 12:40:14 2022"},
    {"Europe/Tallinn", "Sat Jul 30 13:40:14 2022"},
    {"Europe/Kaliningrad", "Sat Jul 30 12:40:14 2022"},
    {"Europe/Podgorica", "Sat Jul 30 12:40:14 2022"},
    {"Europe/Kirov", "Sat Jul 30 13:40:14 2022"},
    {"Europe/Helsinki", "Sat Jul 30 13:40:14 2022"},
    {"Europe/Sarajevo", "Sat Jul 30 12:40:14 2022"},
    {"Europe/Rome", "Sat Jul 30 12:40:14 2022"},
    {"Europe/Bucharest", "Sat Jul 30 13:40:14 2022"},
    {"Europe/Berlin", "Sat Jul 30 12:40:14 2022"},
    {"Europe/Volgograd", "Sat Jul 30 13:40:14 2022"},
    {"Europe/Ljubljana", "Sat Jul 30 12:40:14 2022"},
    {"Europe/Vilnius", "Sat Jul 30 13:40:14 2022"},
    {"Europe/Madrid", "Sat Jul 30 12:40:14 2022"},
    {"Antarctica/Casey", "Sat Jul 30 21:40:14 2022"},
    {"Antarctica/Macquarie", "Sat Jul 30 20:40:14 2022"},
    {"Antarctica/Rothera", "Sat Jul 30 07:40:14 2022"},
    {"Antarctica/Vostok", "Sat Jul 30 16:40:14 2022"},
    {"Antarctica/Davis", "Sat Jul 30 17:40:14 2022"},
    {"Antarctica/Troll", "Sat Jul 30 12:40:14 2022"},
    {"Antarctica/Mawson", "Sat Jul 30 15:40:14 2022"},
    {"Antarctica/McMurdo", "Sat Jul 30 22:40:14 2022"},
    {"Antarctica/DumontDUrville", "Sat Jul 30 20:40:14 2022"},
    {"Antarctica/Palmer", "Sat Jul 30 07:40:14 2022"},
    {"Antarctica/Syowa", "Sat Jul 30 13:40:14 2022"},
    {"WET", "Sat Jul 30 11:40:14 2022"},
    {"CST6CDT", "Sat Jul 30 05:40:14 2022"},
    {"MET", "Sat Jul 30 12:40:14 2022"},
    {"Arctic/Longyearbyen", "Sat Jul 30 12:40:14 2022"},
    {"Atlantic/Faroe", "Sat Jul 30 11:40:14 2022"},
    {"Atlantic/Madeira", "Sat Jul 30 11:40:14 2022"},
    {"Atlantic/Canary", "Sat Jul 30 11:40:14 2022"},
    {"Atlantic/Reykjavik", "Sat Jul 30 10:40:14 2022"},
    {"Atlantic/St_Helena", "Sat Jul 30 10:40:14 2022"},
    {"Atlantic/South_Georgia", "Sat Jul 30 08:40:14 2022"},
    {"Atlantic/Azores", "Sat Jul 30 10:40:14 2022"},
    {"Atlantic/Bermuda", "Sat Jul 30 07:40:14 2022"},
    {"Atlantic/Stanley", "Sat Jul 30 07:40:14 2022"},
    {"Atlantic/Cape_Verde", "Sat Jul 30 09:40:14 2022"},
    {"Africa/Niamey", "Sat Jul 30 11:40:14 2022"},
    {"Africa/Algiers", "Sat Jul 30 11:40:14 2022"},
    {"Africa/Maputo", "Sat Jul 30 12:40:14 2022"},
    {"Africa/Johannesburg", "Sat Jul 30 12:40:14 2022"},
    {"Africa/Addis_Ababa", "Sat Jul 30 13:40:14 2022"},
    {"Africa/Malabo", "Sat Jul 30 11:40:14 2022"},
    {"Africa/Cairo", "Sat Jul 30 12:40:14 2022"},
    {"Africa/Bissau", "Sat Jul 30 10:40:14 2022"},
    {"Africa/Ndjamena", "Sat Jul 30 11:40:14 2022"},
    {"Africa/Tunis", "Sat Jul 30 11:40:14 2022"},
    {"Africa/Harare", "Sat Jul 30 12:40:14 2022"},
    {"Africa/El_Aaiun", "Sat Jul 30 11:40:14 2022"},
    {"Africa/Kampala", "Sat Jul 30 13:40:14 2022"},
    {"Africa/Libreville", "Sat Jul 30 11:40:14 2022"},
    {"Africa/Blantyre", "Sat Jul 30 12:40:14 2022"},
    {"Africa/Brazzaville", "Sat Jul 30 11:40:14 2022"},
    {"Africa/Bangui", "Sat Jul 30 11:40:14 2022"},
    {"Africa/Luanda", "Sat Jul 30 11:40:14 2022"},
    {"Africa/Freetown", "Sat Jul 30 10:40:14 2022"},
    {"Africa/Casablanca", "Sat Jul 30 11:40:14 2022"},
    {"Africa/Conakry", "Sat Jul 30 10:40:14 2022"},
    {"Africa/Kinshasa", "Sat Jul 30 11:40:14 2022"},
    {"Africa/Dakar", "Sat Jul 30 10:40:14 2022"},
    {"Africa/Ouagadougou", "Sat Jul 30 10:40:14 2022"},
    {"Africa/Nouakchott", "Sat Jul 30 10:40:14 2022"},
    {"Africa/Monrovia", "Sat Jul 30 10:40:14 2022"},
    {"Africa/Kigali", "Sat Jul 30 12:40:14 2022"},
    {"Africa/Windhoek", "Sat Jul 30 12:40:14 2022"},
    {"Africa/Juba", "Sat Jul 30 12:40:14 2022"},
    {"Africa/Sao_Tome", "Sat Jul 30 10:40:14 2022"},
    {"Africa/Banjul", "Sat Jul 30 10:40:14 2022"},
    {"Africa/Mbabane", "Sat Jul 30 12:40:14 2022"},
    {"Africa/Asmara", "Sat Jul 30 13:40:14 2022"},
    {"Africa/Djibouti", "Sat Jul 30 13:40:14 2022"},
    {"Africa/Tripoli", "Sat Jul 30 12:40:14 2022"},
    {"Africa/Ceuta", "Sat Jul 30 12:40:14 2022"},
    {"Africa/Khartoum", "Sat Jul 30 12:40:14 2022"},
    {"Africa/Bamako", "Sat Jul 30 10:40:14 2022"},
    {"Africa/Porto-Novo", "Sat Jul 30 11:40:14 2022"},
    {"Africa/Mogadishu", "Sat Jul 30 13:40:14 2022"},
    {"Africa/Abidjan", "Sat Jul 30 10:40:14 2022"},
    {"Africa/Lusaka", "Sat Jul 30 12:40:14 2022"},
    {"Africa/Lagos", "Sat Jul 30 11:40:14 2022"},
    {"Africa/Lubumbashi", "Sat Jul 30 12:40:14 2022"},
    {"Africa/Nairobi", "Sat Jul 30 13:40:14 2022"},
    {"Africa/Accra", "Sat Jul 30 10:40:14 2022"},
    {"Africa/Maseru", "Sat Jul 30 12:40:14 2022"},
    {"Africa/Lome", "Sat Jul 30 10:40:14 2022"},
    {"Africa/Douala", "Sat Jul 30 11:40:14 2022"},
    {"Africa/Bujumbura", "Sat Jul 30 12:40:14 2022"},
    {"Africa/Dar_es_Salaam", "Sat Jul 30 13:40:14 2022"},
    {"Africa/Gaborone", "Sat Jul 30 12:40:14 2022"},
    {"Australia/Adelaide", "Sat Jul 30 20:10:14 2022"},
    {"Australia/Broken_Hill", "Sat Jul 30 20:10:14 2022"},
    {"Australia/Perth", "Sat Jul 30 18:40:14 2022"},
    {"Australia/Darwin", "Sat Jul 30 20:10:14 2022"},
    {"Australia/Lord_Howe", "Sat Jul 30 21:10:14 2022"},
    {"Australia/Hobart", "Sat Jul 30 20:40:14 2022"},
    {"Australia/Brisbane", "Sat Jul 30 20:40:14 2022"},
    {"Australia/Sydney", "Sat Jul 30 20:40:14 2022"},
    {"Australia/Lindeman", "Sat Jul 30 20:40:14 2022"},
    {"Australia/Melbourne", "Sat Jul 30 20:40:14 2022"},
    {"Australia/Eucla", "Sat Jul 30 19:25:14 2022"},
    {"America/Grand_Turk", "Sat Jul 30 06:40:14 2022"},
    {"America/St_Thomas", "Sat Jul 30 06:40:14 2022"},
    {"America/Havana", "Sat Jul 30 06:40:14 2022"},
    {"America/Thunder_Bay", "Sat Jul 30 06:40:14 2022"},
    {"America/Inuvik", "Sat Jul 30 04:40:14 2022"},
    {"America/Winnipeg", "Sat Jul 30 05:40:14 2022"},
    {"America/Maceio", "Sat Jul 30 07:40:14 2022"},
    {"America/Chihuahua", "Sat Jul 30 04:40:14 2022"},
    {"America/North_Dakota/Beulah", "Sat Jul 30 05:40:14 2022"},
    {"America/North_Dakota/Center", "Sat Jul 30 05:40:14 2022"},
    {"America/North_Dakota/New_Salem", "Sat Jul 30 05:40:14 2022"},
    {"America/Campo_Grande", "Sat Jul 30 06:40:14 2022"},
    {"America/Scoresbysund", "Sat Jul 30 10:40:14 2022"},
    {"America/Tijuana", "Sat Jul 30 03:40:14 2022"},
    {"America/Bahia", "Sat Jul 30 07:40:14 2022"},
    {"America/St_Vincent", "Sat Jul 30 06:40:14 2022"},
    {"America/Creston", "Sat Jul 30 03:40:14 2022"},
    {"America/Dawson", "Sat Jul 30 03:40:14 2022"},
    {"America/Tegucigalpa", "Sat Jul 30 04:40:14 2022"},
    {"America/Lima", "Sat Jul 30 05:40:14 2022"},
    {"America/Mexico_City", "Sat Jul 30 05:40:14 2022"},
    {"America/Dominica", "Sat Jul 30 06:40:14 2022"},
    {"America/Atikokan", "Sat Jul 30 05:40:14 2022"},
    {"America/Port-au-Prince", "Sat Jul 30 06:40:14 2022"},
    {"America/Yakutat", "Sat Jul 30 02:40:14 2022"},
    {"America/St_Kitts", "Sat Jul 30 06:40:14 2022"},
    {"America/Guatemala", "Sat Jul 30 04:40:14 2022"},
    {"America/St_Barthelemy", "Sat Jul 30 06:40:14 2022"},
    {"America/Vancouver", "Sat Jul 30 03:40:14 2022"},
    {"America/Jamaica", "Sat Jul 30 05:40:14 2022"},
    {"America/Whitehorse", "Sat Jul 30 03:40:14 2022"},
    {"America/Asuncion", "Sat Jul 30 06:40:14 2022"},
    {"America/Toronto", "Sat Jul 30 06:40:14 2022"},
    {"America/Hermosillo", "Sat Jul 30 03:40:14 2022"},
    {"America/Denver", "Sat Jul 30 04:40:14 2022"},
    {"America/Mazatlan", "Sat Jul 30 04:40:14 2022"},
    {"America/Iqaluit", "Sat Jul 30 06:40:14 2022"},
    {"America/Nuuk", "Sat Jul 30 08:40:14 2022"},
    {"America/Cayenne", "Sat Jul 30 07:40:14 2022"},
    {"America/Santarem", "Sat Jul 30 07:40:14 2022"},
    {"America/Noronha", "Sat Jul 30 08:40:14 2022"},
    {"America/Ojinaga", "Sat Jul 30 04:40:14 2022"},
    {"America/Fort_Nelson", "Sat Jul 30 03:40:14 2022"},
    {"America/Aruba", "Sat Jul 30 06:40:14 2022"},
    {"America/Indiana/Vevay", "Sat Jul 30 06:40:14 2022"},
    {"America/Indiana/Marengo", "Sat Jul 30 06:40:14 2022"},
    {"America/Indiana/Indianapolis", "Sat Jul 30 06:40:14 2022"},
    {"America/Indiana/Tell_City", "Sat Jul 30 05:40:14 2022"},
    {"America/Indiana/Petersburg", "Sat Jul 30 06:40:14 2022"},
    {"America/Indiana/Winamac", "Sat Jul 30 06:40:14 2022"},
    {"America/Indiana/Knox", "Sat Jul 30 05:40:14 2022"},
    {"America/Indiana/Vincennes", "Sat Jul 30 06:40:14 2022"},
    {"America/Cayman", "Sat Jul 30 05:40:14 2022"},
    {"America/Sao_Paulo", "Sat Jul 30 07:40:14 2022"},
    {"America/Curacao", "Sat Jul 30 06:40:14 2022"},
    {"America/Anchorage", "Sat Jul 30 02:40:14 2022"},
    {"America/Guyana", "Sat Jul 30 06:40:14 2022"},
    {"America/Rankin_Inlet", "Sat Jul 30 05:40:14 2022"},
    {"America/Antigua", "Sat Jul 30 06:40:14 2022"},
    {"America/Juneau", "Sat Jul 30 02:40:14 2022"},
    {"America/Merida", "Sat Jul 30 05:40:14 2022"},
    {"America/Punta_Arenas", "Sat Jul 30 07:40:14 2022"},
    {"America/Moncton", "Sat Jul 30 07:40:14 2022"},
    {"America/Fortaleza", "Sat Jul 30 07:40:14 2022"},
    {"America/Resolute", "Sat Jul 30 05:40:14 2022"},
    {"America/Nassau", "Sat Jul 30 06:40:14 2022"},
    {"America/Edmonton", "Sat Jul 30 04:40:14 2022"},
    {"America/Regina", "Sat Jul 30 04:40:14 2022"},
    {"America/Halifax", "Sat Jul 30 07:40:14 2022"},
    {"America/Danmarkshavn", "Sat Jul 30 10:40:14 2022"},
    {"America/Metlakatla", "Sat Jul 30 02:40:14 2022"},
    {"America/Guadeloupe", "Sat Jul 30 06:40:14 2022"},
    {"America/Caracas", "Sat Jul 30 06:40:14 2022"},
    {"America/Detroit", "Sat Jul 30 06:40:14 2022"},
    {"America/Matamoros", "Sat Jul 30 05:40:14 2022"},
    {"America/Glace_Bay", "Sat Jul 30 07:40:14 2022"},
    {"America/Phoenix", "Sat Jul 30 03:40:14 2022"},
    {"America/La_Paz", "Sat Jul 30 06:40:14 2022"},
    {"America/Swift_Current", "Sat Jul 30 04:40:14 2022"},
    {"America/Nipigon", "Sat Jul 30 06:40:14 2022"},
    {"America/Cancun", "Sat Jul 30 05:40:14 2022"},
    {"America/Port_of_Spain", "Sat Jul 30 06:40:14 2022"},
    {"America/Kralendijk", "Sat Jul 30 06:40:14 2022"},
    {"America/Grenada", "Sat Jul 30 06:40:14 2022"},
    {"America/Belem", "Sat Jul 30 07:40:14 2022"},
    {"America/Managua", "Sat Jul 30 04:40:14 2022"},
    {"America/Adak", "Sat Jul 30 01:40:14 2022"},
    {"America/St_Lucia", "Sat Jul 30 06:40:14 2022"},
    {"America/Rio_Branco", "Sat Jul 30 05:40:14 2022"},
    {"America/Santiago", "Sat Jul 30 06:40:14 2022"},
    {"America/Puerto_Rico", "Sat Jul 30 06:40:14 2022"},
    {"America/Miquelon", "Sat Jul 30 08:40:14 2022"},
    {"America/Cuiaba", "Sat Jul 30 06:40:14 2022"},
    {"America/Martinique", "Sat Jul 30 06:40:14 2022"},
    {"America/Argentina/Buenos_Aires", "Sat Jul 30 07:40:14 2022"},
    {"America/Argentina/San_Luis", "Sat Jul 30 07:40:14 2022"},
    {"America/Argentina/Catamarca", "Sat Jul 30 07:40:14 2022"},
    {"America/Argentina/Jujuy", "Sat Jul 30 07:40:14 2022"},
    {"America/Argentina/Salta", "Sat Jul 30 07:40:14 2022"},
    {"America/Argentina/Ushuaia", "Sat Jul 30 07:40:14 2022"},
    {"America/Argentina/Mendoza", "Sat Jul 30 07:40:14 2022"},
    {"America/Argentina/Rio_Gallegos", "Sat Jul 30 07:40:14 2022"},
    {"America/Argentina/Tucuman", "Sat Jul 30 07:40:14 2022"},
    {"America/Argentina/San_Juan", "Sat Jul 30 07:40:14 2022"},
    {"America/Argentina/Cordoba", "Sat Jul 30 07:40:14 2022"},
    {"America/Argentina/La_Rioja", "Sat Jul 30 07:40:14 2022"},
    {"America/Barbados", "Sat Jul 30 06:40:14 2022"},
    {"America/Monterrey", "Sat Jul 30 05:40:14 2022"},
    {"America/Thule", "Sat Jul 30 07:40:14 2022"},
    {"America/Montserrat", "Sat Jul 30 06:40:14 2022"},
    {"America/Guayaquil", "Sat Jul 30 05:40:14 2022"},
    {"America/St_Johns", "Sat Jul 30 08:10:14 2022"},
    {"America/Boise", "Sat Jul 30 04:40:14 2022"},
    {"America/Manaus", "Sat Jul 30 06:40:14 2022"},
    {"America/Nome", "Sat Jul 30 02:40:14 2022"},
    {"America/Goose_Bay", "Sat Jul 30 07:40:14 2022"},
    {"America/Belize", "Sat Jul 30 04:40:14 2022"},
    {"America/Sitka", "Sat Jul 30 02:40:14 2022"},
    {"America/Dawson_Creek", "Sat Jul 30 03:40:14 2022"},
    {"America/Recife", "Sat Jul 30 07:40:14 2022"},
    {"America/Kentucky/Louisville", "Sat Jul 30 06:40:14 2022"},
    {"America/Kentucky/Monticello", "Sat Jul 30 06:40:14 2022"},
    {"America/New_York", "Sat Jul 30 06:40:14 2022"},
    {"America/Tortola", "Sat Jul 30 06:40:14 2022"},
    {"America/Chicago", "Sat Jul 30 05:40:14 2022"},
    {"America/Pangnirtung", "Sat Jul 30 06:40:14 2022"},
    {"America/Boa_Vista", "Sat Jul 30 06:40:14 2022"},
    {"America/Bogota", "Sat Jul 30 05:40:14 2022"},
    {"America/Rainy_River", "Sat Jul 30 05:40:14 2022"},
    {"America/El_Salvador", "Sat Jul 30 04:40:14 2022"},
    {"America/Bahia_Banderas", "Sat Jul 30 05:40:14 2022"},
    {"America/Yellowknife", "Sat Jul 30 04:40:14 2022"},
    {"America/Montevideo", "Sat Jul 30 07:40:14 2022"},
    {"America/Cambridge_Bay", "Sat Jul 30 04:40:14 2022"},
    {"America/Panama", "Sat Jul 30 05:40:14 2022"},
    {"America/Santo_Domingo", "Sat Jul 30 06:40:14 2022"},
    {"America/Anguilla", "Sat Jul 30 06:40:14 2022"},
    {"America/Paramaribo", "Sat Jul 30 07:40:14 2022"},
    {"America/Marigot", "Sat Jul 30 06:40:14 2022"},
    {"America/Menominee", "Sat Jul 30 05:40:14 2022"},
    {"America/Blanc-Sablon", "Sat Jul 30 06:40:14 2022"},
    {"America/Costa_Rica", "Sat Jul 30 04:40:14 2022"},
    {"America/Los_Angeles", "Sat Jul 30 03:40:14 2022"},
    {"America/Lower_Princes", "Sat Jul 30 06:40:14 2022"},
    {"America/Eirunepe", "Sat Jul 30 05:40:14 2022"},
    {"America/Araguaina", "Sat Jul 30 07:40:14 2022"},
    {"America/Porto_Velho", "Sat Jul 30 06:40:14 2022"},
    {"EST", "Sat Jul 30 05:40:14 2022"},
    {"CET", "Sat Jul 30 12:40:14 2022"},
    {"MST7MDT", "Sat Jul 30 04:40:14 2022"},
    {"Indian/Chagos", "Sat Jul 30 16:40:14 2022"},
    {"Indian/Kerguelen", "Sat Jul 30 15:40:14 2022"},
    {"Indian/Reunion", "Sat Jul 30 14:40:14 2022"},
    {"Indian/Antananarivo", "Sat Jul 30 13:40:14 2022"},
    {"Indian/Cocos", "Sat Jul 30 17:10:14 2022"},
    {"Indian/Mauritius", "Sat Jul 30 14:40:14 2022"},
    {"Indian/Christmas", "Sat Jul 30 17:40:14 2022"},
    {"Indian/Maldives", "Sat Jul 30 15:40:14 2022"},
    {"Indian/Comoro", "Sat Jul 30 13:40:14 2022"},
    {"Indian/Mahe", "Sat Jul 30 14:40:14 2022"},
    {"Indian/Mayotte", "Sat Jul 30 13:40:14 2022"},
    {"Asia/Novokuznetsk", "Sat Jul 30 17:40:14 2022"},
    {"Asia/Magadan", "Sat Jul 30 21:40:14 2022"},
    {"Asia/Yangon", "Sat Jul 30 17:10:14 2022"},
    {"Asia/Samarkand", "Sat Jul 30 15:40:14 2022"},
    {"Asia/Tehran", "Sat Jul 30 15:10:14 2022"},
    {"Asia/Dili", "Sat Jul 30 19:40:14 2022"},
    {"Asia/Yerevan", "Sat Jul 30 14:40:14 2022"},
    {"Asia/Qatar", "Sat Jul 30 13:40:14 2022"},
    {"Asia/Chita", "Sat Jul 30 19:40:14 2022"},
    {"Asia/Makassar", "Sat Jul 30 18:40:14 2022"},
    {"Asia/Hovd", "Sat Jul 30 17:40:14 2022"},
    {"Asia/Istanbul", "Sat Jul 30 13:40:14 2022"},
    {"Asia/Irkutsk", "Sat Jul 30 18:40:14 2022"},
    {"Asia/Pontianak", "Sat Jul 30 17:40:14 2022"},
    {"Asia/Qyzylorda", "Sat Jul 30 15:40:14 2022"},
    {"Asia/Srednekolymsk", "Sat Jul 30 21:40:14 2022"},
    {"Asia/Aden", "Sat Jul 30 13:40:14 2022"},
    {"Asia/Kabul", "Sat Jul 30 15:10:14 2022"},
    {"Asia/Macau", "Sat Jul 30 18:40:14 2022"},
    {"Asia/Riyadh", "Sat Jul 30 13:40:14 2022"},
    {"Asia/Taipei", "Sat Jul 30 18:40:14 2022"},
    {"Asia/Manila", "Sat Jul 30 18:40:14 2022"},
    {"Asia/Aqtau", "Sat Jul 30 15:40:14 2022"},
    {"Asia/Thimphu", "Sat Jul 30 16:40:14 2022"},
    {"Asia/Shanghai", "Sat Jul 30 18:40:14 2022"},
    {"Asia/Choibalsan", "Sat Jul 30 18:40:14 2022"},
    {"Asia/Nicosia", "Sat Jul 30 13:40:14 2022"},
    {"Asia/Dubai", "Sat Jul 30 14:40:14 2022"},
    {"Asia/Baku", "Sat Jul 30 14:40:14 2022"},
    {"Asia/Kuching", "Sat Jul 30 18:40:14 2022"},
    {"Asia/Gaza", "Sat Jul 30 13:40:14 2022"},
    {"Asia/Jayapura", "Sat Jul 30 19:40:14 2022"},
    {"Asia/Tokyo", "Sat Jul 30 19:40:14 2022"},
    {"Asia/Muscat", "Sat Jul 30 14:40:14 2022"},
    {"Asia/Kolkata", "Sat Jul 30 16:10:14 2022"},
    {"Asia/Ho_Chi_Minh", "Sat Jul 30 17:40:14 2022"},
    {"Asia/Tashkent", "Sat Jul 30 15:40:14 2022"},
    {"Asia/Bangkok", "Sat Jul 30 17:40:14 2022"},
    {"Asia/Karachi", "Sat Jul 30 15:40:14 2022"},
    {"Asia/Krasnoyarsk", "Sat Jul 30 17:40:14 2022"},
    {"Asia/Yekaterinburg", "Sat Jul 30 15:40:14 2022"},
    {"Asia/Famagusta", "Sat Jul 30 13:40:14 2022"},
    {"Asia/Ulaanbaatar", "Sat Jul 30 18:40:14 2022"},
    {"Asia/Bahrain", "Sat Jul 30 13:40:14 2022"},
    {"Asia/Barnaul", "Sat Jul 30 17:40:14 2022"},
    {"Asia/Jerusalem", "Sat Jul 30 13:40:14 2022"},
    {"Asia/Qostanay", "Sat Jul 30 16:40:14 2022"},
    {"Asia/Ust-Nera", "Sat Jul 30 20:40:14 2022"},
    {"Asia/Tomsk", "Sat Jul 30 17:40:14 2022"},
    {"Asia/Kuala_Lumpur", "Sat Jul 30 18:40:14 2022"},
    {"Asia/Brunei", "Sat Jul 30 18:40:14 2022"},
    {"Asia/Aqtobe", "Sat Jul 30 15:40:14 2022"},
    {"Asia/Vladivostok", "Sat Jul 30 20:40:14 2022"},
    {"Asia/Damascus", "Sat Jul 30 13:40:14 2022"},
    {"Asia/Omsk", "Sat Jul 30 16:40:14 2022"},
    {"Asia/Almaty", "Sat Jul 30 16:40:14 2022"},
    {"Asia/Dhaka", "Sat Jul 30 16:40:14 2022"},
    {"Asia/Tbilisi", "Sat Jul 30 14:40:14 2022"},
    {"Asia/Baghdad", "Sat Jul 30 13:40:14 2022"},
    {"Asia/Phnom_Penh", "Sat Jul 30 17:40:14 2022"},
    {"Asia/Kamchatka", "Sat Jul 30 22:40:14 2022"},
    {"Asia/Ashgabat", "Sat Jul 30 15:40:14 2022"},
    {"Asia/Sakhalin", "Sat Jul 30 21:40:14 2022"},
    {"Asia/Novosibirsk", "Sat Jul 30 17:40:14 2022"},
    {"Asia/Kuwait", "Sat Jul 30 13:40:14 2022"},
    {"Asia/Hong_Kong", "Sat Jul 30 18:40:14 2022"},
    {"Asia/Amman", "Sat Jul 30 13:40:14 2022"},
    {"Asia/Beirut", "Sat Jul 30 13:40:14 2022"},
    {"Asia/Yakutsk", "Sat Jul 30 19:40:14 2022"},
    {"Asia/Colombo", "Sat Jul 30 16:10:14 2022"},
    {"Asia/Oral", "Sat Jul 30 15:40:14 2022"},
    {"Asia/Kathmandu", "Sat Jul 30 16:25:14 2022"},
    {"Asia/Bishkek", "Sat Jul 30 16:40:14 2022"},
    {"Asia/Hebron", "Sat Jul 30 13:40:14 2022"},
    {"Asia/Singapore", "Sat Jul 30 18:40:14 2022"},
    {"Asia/Atyrau", "Sat Jul 30 15:40:14 2022"},
    {"Asia/Khandyga", "Sat Jul 30 19:40:14 2022"},
    {"Asia/Vientiane", "Sat Jul 30 17:40:14 2022"},
    {"Asia/Jakarta", "Sat Jul 30 17:40:14 2022"},
    {"Asia/Dushanbe", "Sat Jul 30 15:40:14 2022"},
    {"Asia/Seoul", "Sat Jul 30 19:40:14 2022"},
    {"Asia/Pyongyang", "Sat Jul 30 19:40:14 2022"},
    {"Asia/Urumqi", "Sat Jul 30 16:40:14 2022"},
    {"Asia/Anadyr", "Sat Jul 30 22:40:14 2022"},
    {"PST8PDT", "Sat Jul 30 03:40:14 2022"},
    {"HST", "Sat Jul 30 00:40:14 2022"},
    {"EST5EDT", "Sat Jul 30 06:40:14 2022"},
};

static const struct strftime_struct gResultDataZ[] = {
    {"Arctic/Longyearbyen", "Sat Jul 30 12:40:14 2022 CEST+0200"},
    {"MST", "Sat Jul 30 03:40:14 2022 MST-0700"},
    {"EST5EDT", "Sat Jul 30 06:40:14 2022 EDT-0400"},
    {"MET", "Sat Jul 30 12:40:14 2022 MEST+0200"},
    {"CET", "Sat Jul 30 12:40:14 2022 CEST+0200"},
    {"Europe/Zagreb", "Sat Jul 30 12:40:14 2022 CEST+0200"},
    {"Europe/Madrid", "Sat Jul 30 12:40:14 2022 CEST+0200"},
    {"Europe/Copenhagen", "Sat Jul 30 12:40:14 2022 CEST+0200"},
    {"Europe/Dublin", "Sat Jul 30 11:40:14 2022 IST+0100"},
    {"Europe/Podgorica", "Sat Jul 30 12:40:14 2022 CEST+0200"},
    {"Europe/Ljubljana", "Sat Jul 30 12:40:14 2022 CEST+0200"},
    {"Europe/Astrakhan", "Sat Jul 30 14:40:14 2022 +04+0400"},
    {"Europe/Nicosia", "Sat Jul 30 13:40:14 2022 EEST+0300"},
    {"Europe/Helsinki", "Sat Jul 30 13:40:14 2022 EEST+0300"},
    {"Europe/Oslo", "Sat Jul 30 12:40:14 2022 CEST+0200"},
    {"Europe/Simferopol", "Sat Jul 30 13:40:14 2022 MSK+0300"},
    {"Europe/Kaliningrad", "Sat Jul 30 12:40:14 2022 EET+0200"},
    {"Europe/Moscow", "Sat Jul 30 13:40:14 2022 MSK+0300"},
    {"Europe/Samara", "Sat Jul 30 14:40:14 2022 +04+0400"},
    {"Europe/Ulyanovsk", "Sat Jul 30 14:40:14 2022 +04+0400"},
    {"Europe/Prague", "Sat Jul 30 12:40:14 2022 CEST+0200"},
    {"Europe/Amsterdam", "Sat Jul 30 12:40:14 2022 CEST+0200"},
    {"Europe/San_Marino", "Sat Jul 30 12:40:14 2022 CEST+0200"},
    {"Europe/Riga", "Sat Jul 30 13:40:14 2022 EEST+0300"},
    {"Europe/Chisinau", "Sat Jul 30 13:40:14 2022 EEST+0300"},
    {"Europe/Athens", "Sat Jul 30 13:40:14 2022 EEST+0300"},
    {"Europe/Mariehamn", "Sat Jul 30 13:40:14 2022 EEST+0300"},
    {"Europe/Minsk", "Sat Jul 30 13:40:14 2022 +03+0300"},
    {"Europe/Berlin", "Sat Jul 30 12:40:14 2022 CEST+0200"},
    {"Europe/Istanbul", "Sat Jul 30 13:40:14 2022 +03+0300"},
    {"Europe/Bucharest", "Sat Jul 30 13:40:14 2022 EEST+0300"},
    {"Europe/Uzhgorod", "Sat Jul 30 13:40:14 2022 EEST+0300"},
    {"Europe/Isle_of_Man", "Sat Jul 30 11:40:14 2022 BST+0100"},
    {"Europe/Tallinn", "Sat Jul 30 13:40:14 2022 EEST+0300"},
    {"Europe/Gibraltar", "Sat Jul 30 12:40:14 2022 CEST+0200"},
    {"Europe/Paris", "Sat Jul 30 12:40:14 2022 CEST+0200"},
    {"Europe/Guernsey", "Sat Jul 30 11:40:14 2022 BST+0100"},
    {"Europe/Zurich", "Sat Jul 30 12:40:14 2022 CEST+0200"},
    {"Europe/Bratislava", "Sat Jul 30 12:40:14 2022 CEST+0200"},
    {"Europe/Vilnius", "Sat Jul 30 13:40:14 2022 EEST+0300"},
    {"Europe/Sofia", "Sat Jul 30 13:40:14 2022 EEST+0300"},
    {"Europe/Vienna", "Sat Jul 30 12:40:14 2022 CEST+0200"},
    {"Europe/Malta", "Sat Jul 30 12:40:14 2022 CEST+0200"},
    {"Europe/Saratov", "Sat Jul 30 14:40:14 2022 +04+0400"},
    {"Europe/Vatican", "Sat Jul 30 12:40:14 2022 CEST+0200"},
    {"Europe/Brussels", "Sat Jul 30 12:40:14 2022 CEST+0200"},
    {"Europe/Tirane", "Sat Jul 30 12:40:14 2022 CEST+0200"},
    {"Europe/Warsaw", "Sat Jul 30 12:40:14 2022 CEST+0200"},
    {"Europe/Kirov", "Sat Jul 30 13:40:14 2022 +03+0300"},
    {"Europe/Kiev", "Sat Jul 30 13:40:14 2022 EEST+0300"},
    {"Europe/Zaporozhye", "Sat Jul 30 13:40:14 2022 EEST+0300"},
    {"Europe/London", "Sat Jul 30 11:40:14 2022 BST+0100"},
    {"Europe/Sarajevo", "Sat Jul 30 12:40:14 2022 CEST+0200"},
    {"Europe/Vaduz", "Sat Jul 30 12:40:14 2022 CEST+0200"},
    {"Europe/Stockholm", "Sat Jul 30 12:40:14 2022 CEST+0200"},
    {"Europe/Belgrade", "Sat Jul 30 12:40:14 2022 CEST+0200"},
    {"Europe/Jersey", "Sat Jul 30 11:40:14 2022 BST+0100"},
    {"Europe/Budapest", "Sat Jul 30 12:40:14 2022 CEST+0200"},
    {"Europe/Volgograd", "Sat Jul 30 13:40:14 2022 +03+0300"},
    {"Europe/Monaco", "Sat Jul 30 12:40:14 2022 CEST+0200"},
    {"Europe/Busingen", "Sat Jul 30 12:40:14 2022 CEST+0200"},
    {"Europe/Andorra", "Sat Jul 30 12:40:14 2022 CEST+0200"},
    {"Europe/Lisbon", "Sat Jul 30 11:40:14 2022 WEST+0100"},
    {"Europe/Skopje", "Sat Jul 30 12:40:14 2022 CEST+0200"},
    {"Europe/Rome", "Sat Jul 30 12:40:14 2022 CEST+0200"},
    {"Europe/Luxembourg", "Sat Jul 30 12:40:14 2022 CEST+0200"},
    {"Australia/Adelaide", "Sat Jul 30 20:10:14 2022 ACST+0930"},
    {"Australia/Darwin", "Sat Jul 30 20:10:14 2022 ACST+0930"},
    {"Australia/Lindeman", "Sat Jul 30 20:40:14 2022 AEST+1000"},
    {"Australia/Melbourne", "Sat Jul 30 20:40:14 2022 AEST+1000"},
    {"Australia/Perth", "Sat Jul 30 18:40:14 2022 AWST+0800"},
    {"Australia/Eucla", "Sat Jul 30 19:25:14 2022 +0845+0845"},
    {"Australia/Sydney", "Sat Jul 30 20:40:14 2022 AEST+1000"},
    {"Australia/Broken_Hill", "Sat Jul 30 20:10:14 2022 ACST+0930"},
    {"Australia/Brisbane", "Sat Jul 30 20:40:14 2022 AEST+1000"},
    {"Australia/Hobart", "Sat Jul 30 20:40:14 2022 AEST+1000"},
    {"Australia/Lord_Howe", "Sat Jul 30 21:10:14 2022 +1030+1030"},
    {"America/Phoenix", "Sat Jul 30 03:40:14 2022 MST-0700"},
    {"America/Belem", "Sat Jul 30 07:40:14 2022 -03-0300"},
    {"America/Kentucky/Monticello", "Sat Jul 30 06:40:14 2022 EDT-0400"},
    {"America/Kentucky/Louisville", "Sat Jul 30 06:40:14 2022 EDT-0400"},
    {"America/Punta_Arenas", "Sat Jul 30 07:40:14 2022 -03-0300"},
    {"America/Antigua", "Sat Jul 30 06:40:14 2022 AST-0400"},
    {"America/Iqaluit", "Sat Jul 30 06:40:14 2022 EDT-0400"},
    {"America/Campo_Grande", "Sat Jul 30 06:40:14 2022 -04-0400"},
    {"America/Lima", "Sat Jul 30 05:40:14 2022 -05-0500"},
    {"America/Moncton", "Sat Jul 30 07:40:14 2022 ADT-0300"},
    {"America/Nuuk", "Sat Jul 30 08:40:14 2022 -02-0200"},
    {"America/Asuncion", "Sat Jul 30 06:40:14 2022 -04-0400"},
    {"America/Edmonton", "Sat Jul 30 04:40:14 2022 MDT-0600"},
    {"America/Tegucigalpa", "Sat Jul 30 04:40:14 2022 CST-0600"},
    {"America/Guyana", "Sat Jul 30 06:40:14 2022 -04-0400"},
    {"America/Argentina/Jujuy", "Sat Jul 30 07:40:14 2022 -03-0300"},
    {"America/Argentina/Salta", "Sat Jul 30 07:40:14 2022 -03-0300"},
    {"America/Argentina/San_Juan", "Sat Jul 30 07:40:14 2022 -03-0300"},
    {"America/Argentina/Mendoza", "Sat Jul 30 07:40:14 2022 -03-0300"},
    {"America/Argentina/Tucuman", "Sat Jul 30 07:40:14 2022 -03-0300"},
    {"America/Argentina/Catamarca", "Sat Jul 30 07:40:14 2022 -03-0300"},
    {"America/Argentina/Cordoba", "Sat Jul 30 07:40:14 2022 -03-0300"},
    {"America/Argentina/Buenos_Aires", "Sat Jul 30 07:40:14 2022 -03-0300"},
    {"America/Argentina/La_Rioja", "Sat Jul 30 07:40:14 2022 -03-0300"},
    {"America/Argentina/San_Luis", "Sat Jul 30 07:40:14 2022 -03-0300"},
    {"America/Argentina/Ushuaia", "Sat Jul 30 07:40:14 2022 -03-0300"},
    {"America/Argentina/Rio_Gallegos", "Sat Jul 30 07:40:14 2022 -03-0300"},
    {"America/Port_of_Spain", "Sat Jul 30 06:40:14 2022 AST-0400"},
    {"America/Boise", "Sat Jul 30 04:40:14 2022 MDT-0600"},
    {"America/Aruba", "Sat Jul 30 06:40:14 2022 AST-0400"},
    {"America/Martinique", "Sat Jul 30 06:40:14 2022 AST-0400"},
    {"America/Matamoros", "Sat Jul 30 05:40:14 2022 CDT-0500"},
    {"America/Guatemala", "Sat Jul 30 04:40:14 2022 CST-0600"},
    {"America/Bogota", "Sat Jul 30 05:40:14 2022 -05-0500"},
    {"America/St_Vincent", "Sat Jul 30 06:40:14 2022 AST-0400"},
    {"America/Managua", "Sat Jul 30 04:40:14 2022 CST-0600"},
    {"America/Montserrat", "Sat Jul 30 06:40:14 2022 AST-0400"},
    {"America/Rio_Branco", "Sat Jul 30 05:40:14 2022 -05-0500"},
    {"America/Tortola", "Sat Jul 30 06:40:14 2022 AST-0400"},
    {"America/El_Salvador", "Sat Jul 30 04:40:14 2022 CST-0600"},
    {"America/Sao_Paulo", "Sat Jul 30 07:40:14 2022 -03-0300"},
    {"America/Whitehorse", "Sat Jul 30 03:40:14 2022 MST-0700"},
    {"America/Dominica", "Sat Jul 30 06:40:14 2022 AST-0400"},
    {"America/Goose_Bay", "Sat Jul 30 07:40:14 2022 ADT-0300"},
    {"America/Sitka", "Sat Jul 30 02:40:14 2022 AKDT-0800"},
    {"America/Panama", "Sat Jul 30 05:40:14 2022 EST-0500"},
    {"America/Regina", "Sat Jul 30 04:40:14 2022 CST-0600"},
    {"America/Fort_Nelson", "Sat Jul 30 03:40:14 2022 MST-0700"},
    {"America/Dawson_Creek", "Sat Jul 30 03:40:14 2022 MST-0700"},
    {"America/La_Paz", "Sat Jul 30 06:40:14 2022 -04-0400"},
    {"America/Thunder_Bay", "Sat Jul 30 06:40:14 2022 EDT-0400"},
    {"America/Bahia_Banderas", "Sat Jul 30 05:40:14 2022 CDT-0500"},
    {"America/Chicago", "Sat Jul 30 05:40:14 2022 CDT-0500"},
    {"America/Mexico_City", "Sat Jul 30 05:40:14 2022 CDT-0500"},
    {"America/Rankin_Inlet", "Sat Jul 30 05:40:14 2022 CDT-0500"},
    {"America/St_Barthelemy", "Sat Jul 30 06:40:14 2022 AST-0400"},
    {"America/Blanc-Sablon", "Sat Jul 30 06:40:14 2022 AST-0400"},
    {"America/Jamaica", "Sat Jul 30 05:40:14 2022 EST-0500"},
    {"America/Cambridge_Bay", "Sat Jul 30 04:40:14 2022 MDT-0600"},
    {"America/Belize", "Sat Jul 30 04:40:14 2022 CST-0600"},
    {"America/Caracas", "Sat Jul 30 06:40:14 2022 -04-0400"},
    {"America/Pangnirtung", "Sat Jul 30 06:40:14 2022 EDT-0400"},
    {"America/Tijuana", "Sat Jul 30 03:40:14 2022 PDT-0700"},
    {"America/Eirunepe", "Sat Jul 30 05:40:14 2022 -05-0500"},
    {"America/Hermosillo", "Sat Jul 30 03:40:14 2022 MST-0700"},
    {"America/Thule", "Sat Jul 30 07:40:14 2022 ADT-0300"},
    {"America/Santo_Domingo", "Sat Jul 30 06:40:14 2022 AST-0400"},
    {"America/Yellowknife", "Sat Jul 30 04:40:14 2022 MDT-0600"},
    {"America/Anguilla", "Sat Jul 30 06:40:14 2022 AST-0400"},
    {"America/Boa_Vista", "Sat Jul 30 06:40:14 2022 -04-0400"},
    {"America/Grand_Turk", "Sat Jul 30 06:40:14 2022 EDT-0400"},
    {"America/Ojinaga", "Sat Jul 30 04:40:14 2022 MDT-0600"},
    {"America/Bahia", "Sat Jul 30 07:40:14 2022 -03-0300"},
    {"America/Inuvik", "Sat Jul 30 04:40:14 2022 MDT-0600"},
    {"America/Yakutat", "Sat Jul 30 02:40:14 2022 AKDT-0800"},
    {"America/Santiago", "Sat Jul 30 06:40:14 2022 -04-0400"},
    {"America/Merida", "Sat Jul 30 05:40:14 2022 CDT-0500"},
    {"America/North_Dakota/New_Salem", "Sat Jul 30 05:40:14 2022 CDT-0500"},
    {"America/North_Dakota/Beulah", "Sat Jul 30 05:40:14 2022 CDT-0500"},
    {"America/North_Dakota/Center", "Sat Jul 30 05:40:14 2022 CDT-0500"},
    {"America/Guayaquil", "Sat Jul 30 05:40:14 2022 -05-0500"},
    {"America/Toronto", "Sat Jul 30 06:40:14 2022 EDT-0400"},
    {"America/Curacao", "Sat Jul 30 06:40:14 2022 AST-0400"},
    {"America/Puerto_Rico", "Sat Jul 30 06:40:14 2022 AST-0400"},
    {"America/Vancouver", "Sat Jul 30 03:40:14 2022 PDT-0700"},
    {"America/Cayman", "Sat Jul 30 05:40:14 2022 EST-0500"},
    {"America/Adak", "Sat Jul 30 01:40:14 2022 HDT-0900"},
    {"America/Mazatlan", "Sat Jul 30 04:40:14 2022 MDT-0600"},
    {"America/Anchorage", "Sat Jul 30 02:40:14 2022 AKDT-0800"},
    {"America/Porto_Velho", "Sat Jul 30 06:40:14 2022 -04-0400"},
    {"America/Denver", "Sat Jul 30 04:40:14 2022 MDT-0600"},
    {"America/New_York", "Sat Jul 30 06:40:14 2022 EDT-0400"},
    {"America/Los_Angeles", "Sat Jul 30 03:40:14 2022 PDT-0700"},
    {"America/St_Johns", "Sat Jul 30 08:10:14 2022 NDT-0230"},
    {"America/Nassau", "Sat Jul 30 06:40:14 2022 EDT-0400"},
    {"America/Menominee", "Sat Jul 30 05:40:14 2022 CDT-0500"},
    {"America/Dawson", "Sat Jul 30 03:40:14 2022 MST-0700"},
    {"America/Danmarkshavn", "Sat Jul 30 10:40:14 2022 GMT+0000"},
    {"America/Metlakatla", "Sat Jul 30 02:40:14 2022 AKDT-0800"},
    {"America/Nome", "Sat Jul 30 02:40:14 2022 AKDT-0800"},
    {"America/Rainy_River", "Sat Jul 30 05:40:14 2022 CDT-0500"},
    {"America/Winnipeg", "Sat Jul 30 05:40:14 2022 CDT-0500"},
    {"America/St_Kitts", "Sat Jul 30 06:40:14 2022 AST-0400"},
    {"America/Cayenne", "Sat Jul 30 07:40:14 2022 -03-0300"},
    {"America/Cuiaba", "Sat Jul 30 06:40:14 2022 -04-0400"},
    {"America/Costa_Rica", "Sat Jul 30 04:40:14 2022 CST-0600"},
    {"America/Paramaribo", "Sat Jul 30 07:40:14 2022 -03-0300"},
    {"America/Guadeloupe", "Sat Jul 30 06:40:14 2022 AST-0400"},
    {"America/Miquelon", "Sat Jul 30 08:40:14 2022 -02-0200"},
    {"America/St_Lucia", "Sat Jul 30 06:40:14 2022 AST-0400"},
    {"America/Noronha", "Sat Jul 30 08:40:14 2022 -02-0200"},
    {"America/Resolute", "Sat Jul 30 05:40:14 2022 CDT-0500"},
    {"America/Grenada", "Sat Jul 30 06:40:14 2022 AST-0400"},
    {"America/Barbados", "Sat Jul 30 06:40:14 2022 AST-0400"},
    {"America/Juneau", "Sat Jul 30 02:40:14 2022 AKDT-0800"},
    {"America/Havana", "Sat Jul 30 06:40:14 2022 CDT-0400"},
    {"America/Maceio", "Sat Jul 30 07:40:14 2022 -03-0300"},
    {"America/Scoresbysund", "Sat Jul 30 10:40:14 2022 +00+0000"},
    {"America/Indiana/Tell_City", "Sat Jul 30 05:40:14 2022 CDT-0500"},
    {"America/Indiana/Petersburg", "Sat Jul 30 06:40:14 2022 EDT-0400"},
    {"America/Indiana/Vevay", "Sat Jul 30 06:40:14 2022 EDT-0400"},
    {"America/Indiana/Indianapolis", "Sat Jul 30 06:40:14 2022 EDT-0400"},
    {"America/Indiana/Knox", "Sat Jul 30 05:40:14 2022 CDT-0500"},
    {"America/Indiana/Marengo", "Sat Jul 30 06:40:14 2022 EDT-0400"},
    {"America/Indiana/Vincennes", "Sat Jul 30 06:40:14 2022 EDT-0400"},
    {"America/Indiana/Winamac", "Sat Jul 30 06:40:14 2022 EDT-0400"},
    {"America/Monterrey", "Sat Jul 30 05:40:14 2022 CDT-0500"},
    {"America/Recife", "Sat Jul 30 07:40:14 2022 -03-0300"},
    {"America/Detroit", "Sat Jul 30 06:40:14 2022 EDT-0400"},
    {"America/Manaus", "Sat Jul 30 06:40:14 2022 -04-0400"},
    {"America/Swift_Current", "Sat Jul 30 04:40:14 2022 CST-0600"},
    {"America/Nipigon", "Sat Jul 30 06:40:14 2022 EDT-0400"},
    {"America/Marigot", "Sat Jul 30 06:40:14 2022 AST-0400"},
    {"America/St_Thomas", "Sat Jul 30 06:40:14 2022 AST-0400"},
    {"America/Atikokan", "Sat Jul 30 05:40:14 2022 EST-0500"},
    {"America/Halifax", "Sat Jul 30 07:40:14 2022 ADT-0300"},
    {"America/Cancun", "Sat Jul 30 05:40:14 2022 EST-0500"},
    {"America/Araguaina", "Sat Jul 30 07:40:14 2022 -03-0300"},
    {"America/Lower_Princes", "Sat Jul 30 06:40:14 2022 AST-0400"},
    {"America/Creston", "Sat Jul 30 03:40:14 2022 MST-0700"},
    {"America/Kralendijk", "Sat Jul 30 06:40:14 2022 AST-0400"},
    {"America/Glace_Bay", "Sat Jul 30 07:40:14 2022 ADT-0300"},
    {"America/Fortaleza", "Sat Jul 30 07:40:14 2022 -03-0300"},
    {"America/Santarem", "Sat Jul 30 07:40:14 2022 -03-0300"},
    {"America/Chihuahua", "Sat Jul 30 04:40:14 2022 MDT-0600"},
    {"America/Port-au-Prince", "Sat Jul 30 06:40:14 2022 EDT-0400"},
    {"America/Montevideo", "Sat Jul 30 07:40:14 2022 -03-0300"},
    {"HST", "Sat Jul 30 00:40:14 2022 HST-1000"},
    {"Asia/Irkutsk", "Sat Jul 30 18:40:14 2022 +08+0800"},
    {"Asia/Almaty", "Sat Jul 30 16:40:14 2022 +06+0600"},
    {"Asia/Phnom_Penh", "Sat Jul 30 17:40:14 2022 +07+0700"},
    {"Asia/Hong_Kong", "Sat Jul 30 18:40:14 2022 HKT+0800"},
    {"Asia/Dushanbe", "Sat Jul 30 15:40:14 2022 +05+0500"},
    {"Asia/Yakutsk", "Sat Jul 30 19:40:14 2022 +09+0900"},
    {"Asia/Aqtobe", "Sat Jul 30 15:40:14 2022 +05+0500"},
    {"Asia/Nicosia", "Sat Jul 30 13:40:14 2022 EEST+0300"},
    {"Asia/Pontianak", "Sat Jul 30 17:40:14 2022 WIB+0700"},
    {"Asia/Tbilisi", "Sat Jul 30 14:40:14 2022 +04+0400"},
    {"Asia/Tehran", "Sat Jul 30 15:10:14 2022 +0430+0430"},
    {"Asia/Tashkent", "Sat Jul 30 15:40:14 2022 +05+0500"},
    {"Asia/Jayapura", "Sat Jul 30 19:40:14 2022 WIT+0900"},
    {"Asia/Beirut", "Sat Jul 30 13:40:14 2022 EEST+0300"},
    {"Asia/Sakhalin", "Sat Jul 30 21:40:14 2022 +11+1100"},
    {"Asia/Anadyr", "Sat Jul 30 22:40:14 2022 +12+1200"},
    {"Asia/Yekaterinburg", "Sat Jul 30 15:40:14 2022 +05+0500"},
    {"Asia/Urumqi", "Sat Jul 30 16:40:14 2022 +06+0600"},
    {"Asia/Damascus", "Sat Jul 30 13:40:14 2022 EEST+0300"},
    {"Asia/Shanghai", "Sat Jul 30 18:40:14 2022 CST+0800"},
    {"Asia/Baghdad", "Sat Jul 30 13:40:14 2022 +03+0300"},
    {"Asia/Ho_Chi_Minh", "Sat Jul 30 17:40:14 2022 +07+0700"},
    {"Asia/Famagusta", "Sat Jul 30 13:40:14 2022 EEST+0300"},
    {"Asia/Barnaul", "Sat Jul 30 17:40:14 2022 +07+0700"},
    {"Asia/Tomsk", "Sat Jul 30 17:40:14 2022 +07+0700"},
    {"Asia/Samarkand", "Sat Jul 30 15:40:14 2022 +05+0500"},
    {"Asia/Oral", "Sat Jul 30 15:40:14 2022 +05+0500"},
    {"Asia/Hovd", "Sat Jul 30 17:40:14 2022 +07+0700"},
    {"Asia/Qatar", "Sat Jul 30 13:40:14 2022 +03+0300"},
    {"Asia/Macau", "Sat Jul 30 18:40:14 2022 CST+0800"},
    {"Asia/Kathmandu", "Sat Jul 30 16:25:14 2022 +0545+0545"},
    {"Asia/Krasnoyarsk", "Sat Jul 30 17:40:14 2022 +07+0700"},
    {"Asia/Istanbul", "Sat Jul 30 13:40:14 2022 +03+0300"},
    {"Asia/Ashgabat", "Sat Jul 30 15:40:14 2022 +05+0500"},
    {"Asia/Omsk", "Sat Jul 30 16:40:14 2022 +06+0600"},
    {"Asia/Amman", "Sat Jul 30 13:40:14 2022 EEST+0300"},
    {"Asia/Chita", "Sat Jul 30 19:40:14 2022 +09+0900"},
    {"Asia/Singapore", "Sat Jul 30 18:40:14 2022 +08+0800"},
    {"Asia/Vladivostok", "Sat Jul 30 20:40:14 2022 +10+1000"},
    {"Asia/Kuching", "Sat Jul 30 18:40:14 2022 +08+0800"},
    {"Asia/Bahrain", "Sat Jul 30 13:40:14 2022 +03+0300"},
    {"Asia/Choibalsan", "Sat Jul 30 18:40:14 2022 +08+0800"},
    {"Asia/Kolkata", "Sat Jul 30 16:10:14 2022 IST+0530"},
    {"Asia/Colombo", "Sat Jul 30 16:10:14 2022 +0530+0530"},
    {"Asia/Ulaanbaatar", "Sat Jul 30 18:40:14 2022 +08+0800"},
    {"Asia/Qyzylorda", "Sat Jul 30 15:40:14 2022 +05+0500"},
    {"Asia/Tokyo", "Sat Jul 30 19:40:14 2022 JST+0900"},
    {"Asia/Yerevan", "Sat Jul 30 14:40:14 2022 +04+0400"},
    {"Asia/Kamchatka", "Sat Jul 30 22:40:14 2022 +12+1200"},
    {"Asia/Ust-Nera", "Sat Jul 30 20:40:14 2022 +10+1000"},
    {"Asia/Vientiane", "Sat Jul 30 17:40:14 2022 +07+0700"},
    {"Asia/Taipei", "Sat Jul 30 18:40:14 2022 CST+0800"},
    {"Asia/Baku", "Sat Jul 30 14:40:14 2022 +04+0400"},
    {"Asia/Makassar", "Sat Jul 30 18:40:14 2022 WITA+0800"},
    {"Asia/Novosibirsk", "Sat Jul 30 17:40:14 2022 +07+0700"},
    {"Asia/Riyadh", "Sat Jul 30 13:40:14 2022 +03+0300"},
    {"Asia/Aden", "Sat Jul 30 13:40:14 2022 +03+0300"},
    {"Asia/Bangkok", "Sat Jul 30 17:40:14 2022 +07+0700"},
    {"Asia/Magadan", "Sat Jul 30 21:40:14 2022 +11+1100"},
    {"Asia/Dubai", "Sat Jul 30 14:40:14 2022 +04+0400"},
    {"Asia/Kuwait", "Sat Jul 30 13:40:14 2022 +03+0300"},
    {"Asia/Novokuznetsk", "Sat Jul 30 17:40:14 2022 +07+0700"},
    {"Asia/Aqtau", "Sat Jul 30 15:40:14 2022 +05+0500"},
    {"Asia/Thimphu", "Sat Jul 30 16:40:14 2022 +06+0600"},
    {"Asia/Hebron", "Sat Jul 30 13:40:14 2022 EEST+0300"},
    {"Asia/Brunei", "Sat Jul 30 18:40:14 2022 +08+0800"},
    {"Asia/Khandyga", "Sat Jul 30 19:40:14 2022 +09+0900"},
    {"Asia/Kuala_Lumpur", "Sat Jul 30 18:40:14 2022 +08+0800"},
    {"Asia/Srednekolymsk", "Sat Jul 30 21:40:14 2022 +11+1100"},
    {"Asia/Dili", "Sat Jul 30 19:40:14 2022 +09+0900"},
    {"Asia/Karachi", "Sat Jul 30 15:40:14 2022 PKT+0500"},
    {"Asia/Muscat", "Sat Jul 30 14:40:14 2022 +04+0400"},
    {"Asia/Qostanay", "Sat Jul 30 16:40:14 2022 +06+0600"},
    {"Asia/Dhaka", "Sat Jul 30 16:40:14 2022 +06+0600"},
    {"Asia/Kabul", "Sat Jul 30 15:10:14 2022 +0430+0430"},
    {"Asia/Jakarta", "Sat Jul 30 17:40:14 2022 WIB+0700"},
    {"Asia/Manila", "Sat Jul 30 18:40:14 2022 PST+0800"},
    {"Asia/Gaza", "Sat Jul 30 13:40:14 2022 EEST+0300"},
    {"Asia/Bishkek", "Sat Jul 30 16:40:14 2022 +06+0600"},
    {"Asia/Jerusalem", "Sat Jul 30 13:40:14 2022 IDT+0300"},
    {"Asia/Atyrau", "Sat Jul 30 15:40:14 2022 +05+0500"},
    {"Asia/Pyongyang", "Sat Jul 30 19:40:14 2022 KST+0900"},
    {"Asia/Yangon", "Sat Jul 30 17:10:14 2022 +0630+0630"},
    {"Asia/Seoul", "Sat Jul 30 19:40:14 2022 KST+0900"},
    {"WET", "Sat Jul 30 11:40:14 2022 WEST+0100"},
    {"Indian/Mayotte", "Sat Jul 30 13:40:14 2022 EAT+0300"},
    {"Indian/Comoro", "Sat Jul 30 13:40:14 2022 EAT+0300"},
    {"Indian/Chagos", "Sat Jul 30 16:40:14 2022 +06+0600"},
    {"Indian/Reunion", "Sat Jul 30 14:40:14 2022 +04+0400"},
    {"Indian/Mahe", "Sat Jul 30 14:40:14 2022 +04+0400"},
    {"Indian/Christmas", "Sat Jul 30 17:40:14 2022 +07+0700"},
    {"Indian/Mauritius", "Sat Jul 30 14:40:14 2022 +04+0400"},
    {"Indian/Maldives", "Sat Jul 30 15:40:14 2022 +05+0500"},
    {"Indian/Kerguelen", "Sat Jul 30 15:40:14 2022 +05+0500"},
    {"Indian/Antananarivo", "Sat Jul 30 13:40:14 2022 EAT+0300"},
    {"Indian/Cocos", "Sat Jul 30 17:10:14 2022 +0630+0630"},
    {"CST6CDT", "Sat Jul 30 05:40:14 2022 CDT-0500"},
    {"EST", "Sat Jul 30 05:40:14 2022 EST-0500"},
    {"Africa/Bujumbura", "Sat Jul 30 12:40:14 2022 CAT+0200"},
    {"Africa/Casablanca", "Sat Jul 30 11:40:14 2022 +01+0100"},
    {"Africa/Lome", "Sat Jul 30 10:40:14 2022 GMT+0000"},
    {"Africa/Monrovia", "Sat Jul 30 10:40:14 2022 GMT+0000"},
    {"Africa/Dar_es_Salaam", "Sat Jul 30 13:40:14 2022 EAT+0300"},
    {"Africa/Lagos", "Sat Jul 30 11:40:14 2022 WAT+0100"},
    {"Africa/Tripoli", "Sat Jul 30 12:40:14 2022 EET+0200"},
    {"Africa/Tunis", "Sat Jul 30 11:40:14 2022 CET+0100"},
    {"Africa/Nairobi", "Sat Jul 30 13:40:14 2022 EAT+0300"},
    {"Africa/Banjul", "Sat Jul 30 10:40:14 2022 GMT+0000"},
    {"Africa/Djibouti", "Sat Jul 30 13:40:14 2022 EAT+0300"},
    {"Africa/Luanda", "Sat Jul 30 11:40:14 2022 WAT+0100"},
    {"Africa/Lusaka", "Sat Jul 30 12:40:14 2022 CAT+0200"},
    {"Africa/Maputo", "Sat Jul 30 12:40:14 2022 CAT+0200"},
    {"Africa/Juba", "Sat Jul 30 12:40:14 2022 CAT+0200"},
    {"Africa/Harare", "Sat Jul 30 12:40:14 2022 CAT+0200"},
    {"Africa/Libreville", "Sat Jul 30 11:40:14 2022 WAT+0100"},
    {"Africa/Khartoum", "Sat Jul 30 12:40:14 2022 CAT+0200"},
    {"Africa/Bangui", "Sat Jul 30 11:40:14 2022 WAT+0100"},
    {"Africa/Lubumbashi", "Sat Jul 30 12:40:14 2022 CAT+0200"},
    {"Africa/Brazzaville", "Sat Jul 30 11:40:14 2022 WAT+0100"},
    {"Africa/Cairo", "Sat Jul 30 12:40:14 2022 EET+0200"},
    {"Africa/Ouagadougou", "Sat Jul 30 10:40:14 2022 GMT+0000"},
    {"Africa/Addis_Ababa", "Sat Jul 30 13:40:14 2022 EAT+0300"},
    {"Africa/Accra", "Sat Jul 30 10:40:14 2022 GMT+0000"},
    {"Africa/Windhoek", "Sat Jul 30 12:40:14 2022 CAT+0200"},
    {"Africa/Malabo", "Sat Jul 30 11:40:14 2022 WAT+0100"},
    {"Africa/Mbabane", "Sat Jul 30 12:40:14 2022 SAST+0200"},
    {"Africa/Ceuta", "Sat Jul 30 12:40:14 2022 CEST+0200"},
    {"Africa/Maseru", "Sat Jul 30 12:40:14 2022 SAST+0200"},
    {"Africa/Bamako", "Sat Jul 30 10:40:14 2022 GMT+0000"},
    {"Africa/Conakry", "Sat Jul 30 10:40:14 2022 GMT+0000"},
    {"Africa/Douala", "Sat Jul 30 11:40:14 2022 WAT+0100"},
    {"Africa/Dakar", "Sat Jul 30 10:40:14 2022 GMT+0000"},
    {"Africa/Freetown", "Sat Jul 30 10:40:14 2022 GMT+0000"},
    {"Africa/Bissau", "Sat Jul 30 10:40:14 2022 GMT+0000"},
    {"Africa/Sao_Tome", "Sat Jul 30 10:40:14 2022 GMT+0000"},
    {"Africa/Kinshasa", "Sat Jul 30 11:40:14 2022 WAT+0100"},
    {"Africa/Abidjan", "Sat Jul 30 10:40:14 2022 GMT+0000"},
    {"Africa/El_Aaiun", "Sat Jul 30 11:40:14 2022 +01+0100"},
    {"Africa/Gaborone", "Sat Jul 30 12:40:14 2022 CAT+0200"},
    {"Africa/Kampala", "Sat Jul 30 13:40:14 2022 EAT+0300"},
    {"Africa/Porto-Novo", "Sat Jul 30 11:40:14 2022 WAT+0100"},
    {"Africa/Nouakchott", "Sat Jul 30 10:40:14 2022 GMT+0000"},
    {"Africa/Algiers", "Sat Jul 30 11:40:14 2022 CET+0100"},
    {"Africa/Ndjamena", "Sat Jul 30 11:40:14 2022 WAT+0100"},
    {"Africa/Mogadishu", "Sat Jul 30 13:40:14 2022 EAT+0300"},
    {"Africa/Niamey", "Sat Jul 30 11:40:14 2022 WAT+0100"},
    {"Africa/Blantyre", "Sat Jul 30 12:40:14 2022 CAT+0200"},
    {"Africa/Kigali", "Sat Jul 30 12:40:14 2022 CAT+0200"},
    {"Africa/Johannesburg", "Sat Jul 30 12:40:14 2022 SAST+0200"},
    {"Africa/Asmara", "Sat Jul 30 13:40:14 2022 EAT+0300"},
    {"Atlantic/Reykjavik", "Sat Jul 30 10:40:14 2022 GMT+0000"},
    {"Atlantic/Faroe", "Sat Jul 30 11:40:14 2022 WEST+0100"},
    {"Atlantic/Cape_Verde", "Sat Jul 30 09:40:14 2022 -01-0100"},
    {"Atlantic/St_Helena", "Sat Jul 30 10:40:14 2022 GMT+0000"},
    {"Atlantic/Bermuda", "Sat Jul 30 07:40:14 2022 ADT-0300"},
    {"Atlantic/Stanley", "Sat Jul 30 07:40:14 2022 -03-0300"},
    {"Atlantic/Madeira", "Sat Jul 30 11:40:14 2022 WEST+0100"},
    {"Atlantic/Canary", "Sat Jul 30 11:40:14 2022 WEST+0100"},
    {"Atlantic/South_Georgia", "Sat Jul 30 08:40:14 2022 -02-0200"},
    {"Atlantic/Azores", "Sat Jul 30 10:40:14 2022 +00+0000"},
    {"MST7MDT", "Sat Jul 30 04:40:14 2022 MDT-0600"},
    {"Antarctica/Mawson", "Sat Jul 30 15:40:14 2022 +05+0500"},
    {"Antarctica/Macquarie", "Sat Jul 30 20:40:14 2022 AEST+1000"},
    {"Antarctica/DumontDUrville", "Sat Jul 30 20:40:14 2022 +10+1000"},
    {"Antarctica/Davis", "Sat Jul 30 17:40:14 2022 +07+0700"},
    {"Antarctica/Rothera", "Sat Jul 30 07:40:14 2022 -03-0300"},
    {"Antarctica/Casey", "Sat Jul 30 21:40:14 2022 +11+1100"},
    {"Antarctica/Vostok", "Sat Jul 30 16:40:14 2022 +06+0600"},
    {"Antarctica/Palmer", "Sat Jul 30 07:40:14 2022 -03-0300"},
    {"Antarctica/McMurdo", "Sat Jul 30 22:40:14 2022 NZST+1200"},
    {"Antarctica/Troll", "Sat Jul 30 12:40:14 2022 +02+0200"},
    {"Antarctica/Syowa", "Sat Jul 30 13:40:14 2022 +03+0300"},
    {"PST8PDT", "Sat Jul 30 03:40:14 2022 PDT-0700"},
    {"EET", "Sat Jul 30 13:40:14 2022 EEST+0300"},
    {"Pacific/Port_Moresby", "Sat Jul 30 20:40:14 2022 +10+1000"},
    {"Pacific/Rarotonga", "Sat Jul 30 00:40:14 2022 -10-1000"},
    {"Pacific/Efate", "Sat Jul 30 21:40:14 2022 +11+1100"},
    {"Pacific/Midway", "Fri Jul 29 23:40:14 2022 SST-1100"},
    {"Pacific/Gambier", "Sat Jul 30 01:40:14 2022 -09-0900"},
    {"Pacific/Wake", "Sat Jul 30 22:40:14 2022 +12+1200"},
    {"Pacific/Pago_Pago", "Fri Jul 29 23:40:14 2022 SST-1100"},
    {"Pacific/Fiji", "Sat Jul 30 22:40:14 2022 +12+1200"},
    {"Pacific/Auckland", "Sat Jul 30 22:40:14 2022 NZST+1200"},
    {"Pacific/Chatham", "Sat Jul 30 23:25:14 2022 +1245+1245"},
    {"Pacific/Pitcairn", "Sat Jul 30 02:40:14 2022 -08-0800"},
    {"Pacific/Galapagos", "Sat Jul 30 04:40:14 2022 -06-0600"},
    {"Pacific/Tahiti", "Sat Jul 30 00:40:14 2022 -10-1000"},
    {"Pacific/Tongatapu", "Sat Jul 30 23:40:14 2022 +13+1300"},
    {"Pacific/Kwajalein", "Sat Jul 30 22:40:14 2022 +12+1200"},
    {"Pacific/Chuuk", "Sat Jul 30 20:40:14 2022 +10+1000"},
    {"Pacific/Fakaofo", "Sat Jul 30 23:40:14 2022 +13+1300"},
    {"Pacific/Honolulu", "Sat Jul 30 00:40:14 2022 HST-1000"},
    {"Pacific/Saipan", "Sat Jul 30 20:40:14 2022 ChST+1000"},
    {"Pacific/Guadalcanal", "Sat Jul 30 21:40:14 2022 +11+1100"},
    {"Pacific/Nauru", "Sat Jul 30 22:40:14 2022 +12+1200"},
    {"Pacific/Enderbury", "Sat Jul 30 23:40:14 2022 +13+1300"},
    {"Pacific/Bougainville", "Sat Jul 30 21:40:14 2022 +11+1100"},
    {"Pacific/Tarawa", "Sat Jul 30 22:40:14 2022 +12+1200"},
    {"Pacific/Kiritimati", "Sun Jul 31 00:40:14 2022 +14+1400"},
    {"Pacific/Apia", "Sat Jul 30 23:40:14 2022 +13+1300"},
    {"Pacific/Niue", "Fri Jul 29 23:40:14 2022 -11-1100"},
    {"Pacific/Marquesas", "Sat Jul 30 01:10:14 2022 -0930-0930"},
    {"Pacific/Kosrae", "Sat Jul 30 21:40:14 2022 +11+1100"},
    {"Pacific/Funafuti", "Sat Jul 30 22:40:14 2022 +12+1200"},
    {"Pacific/Easter", "Sat Jul 30 04:40:14 2022 -06-0600"},
    {"Pacific/Noumea", "Sat Jul 30 21:40:14 2022 +11+1100"},
    {"Pacific/Norfolk", "Sat Jul 30 21:40:14 2022 +11+1100"},
    {"Pacific/Guam", "Sat Jul 30 20:40:14 2022 ChST+1000"},
    {"Pacific/Wallis", "Sat Jul 30 22:40:14 2022 +12+1200"},
    {"Pacific/Pohnpei", "Sat Jul 30 21:40:14 2022 +11+1100"},
    {"Pacific/Palau", "Sat Jul 30 19:40:14 2022 +09+0900"},
    {"Pacific/Majuro", "Sat Jul 30 22:40:14 2022 +12+1200"},
};

static time_t gTime = 1659177614;
static int16_t gBufferSize = 256;

/**
 * @tc.name      : strftime_0100
 * @tc.desc      : according to different time zones, format date
 * @tc.level     : Level 0
 */
void strftime_0100(void)
{
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
        char buffer[gBufferSize];
        struct tm *timeptr = localtime(&gTime);
        if (!timeptr) {
            EXPECT_PTRNE("strftime_0100", timeptr, NULL);
            return;
        }
        size_t count = strftime(buffer, sizeof(buffer) - 1, "%c", timeptr);
        EXPECT_TRUE("strftime_0100", count > 0);
        EXPECT_STREQ("strftime_0100", buffer, gResultData[i].result);
    }
}

/**
 * @tc.name      : strftime_0200
 * @tc.desc      : according to different time zones, format date
 * @tc.level     : Level 0
 */
void strftime_0200(void)
{
    for (int32_t i = 0; i < (int32_t)(sizeof(gResultDataZ) / sizeof(gResultDataZ[0])); i++) {
        const char *tz = gResultDataZ[i].tz;
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
        struct tm *timeptr = localtime(&gTime);
        if (!timeptr) {
            EXPECT_PTRNE("strftime_0200", timeptr, NULL);
            return;
        }
        char buffer[gBufferSize];
        size_t count = strftime(buffer, sizeof(buffer) - 1, "%c %Z%z", timeptr);
        EXPECT_TRUE("strftime_0200", count > 0);
        EXPECT_STREQ("strftime_0200", buffer, gResultDataZ[i].result);
    }
}

/**
 * @tc.name      : strftime_0300
 * @tc.desc      : according to different time zones, format date
 * @tc.level     : Level 0
 */
void strftime_0300(void)
{
    const char *tz = "Pacific/Pitcairn";
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
    struct tm *timeptr = localtime(&gTime);
    if (!timeptr) {
        EXPECT_PTRNE("strftime_0300", timeptr, NULL);
        return;
    }
    char buffer[gBufferSize];
    size_t count = strftime(buffer, sizeof(buffer) - 1, "%k", timeptr);
    EXPECT_TRUE("strftime_0300", count > 0);
    EXPECT_STREQ("strftime_0300", buffer, " 2");
}

/**
 * @tc.name      : strftime_0400
 * @tc.desc      : according to different time zones, format date
 * @tc.level     : Level 0
 */
void strftime_0400(void)
{
    const char *tz = "Asia/Shanghai";
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
    struct tm *timeptr = localtime(&gTime);
    if (!timeptr) {
        EXPECT_PTRNE("strftime_0400", timeptr, NULL);
        return;
    }
    char buffer[gBufferSize];
    size_t count = strftime(buffer, sizeof(buffer) - 1, "%k", timeptr);
    EXPECT_TRUE("strftime_0400", count > 0);
    EXPECT_STREQ("strftime_0400", buffer, "18");
}

/**
 * @tc.name      : strftime_0500
 * @tc.desc      : according to different time zones, format date
 * @tc.level     : Level 0
 */
void strftime_0500(void)
{
    const char *tz = "Asia/Shanghai";
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
    struct tm *timeptr = localtime(&gTime);
    if (!timeptr) {
        EXPECT_PTRNE("strftime_0500", timeptr, NULL);
        return;
    }
    char buffer[gBufferSize];
    size_t count = strftime(buffer, sizeof(buffer) - 1, "%I", timeptr);
    EXPECT_TRUE("strftime_0500", count > 0);
    EXPECT_STREQ("strftime_0500", buffer, "06");
}

/**
 * @tc.name      : strftime_0600
 * @tc.desc      : according to different time zones, format date
 * @tc.level     : Level 0
 */
void strftime_0600(void)
{
    const char *tz = "Asia/Shanghai";
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
    struct tm *timeptr = localtime(&gTime);
    if (!timeptr) {
        EXPECT_PTRNE("strftime_0600", timeptr, NULL);
        return;
    }
    char buffer[gBufferSize];
    size_t count = strftime(buffer, sizeof(buffer) - 1, "%P", timeptr);
    EXPECT_TRUE("strftime_0600", count > 0);
    EXPECT_STREQ("strftime_0600", buffer, "pm");
}

/**
 * @tc.name      : strftime_0700
 * @tc.desc      : according to different time zones, format date
 * @tc.level     : Level 0
 */
void strftime_0700(void)
{
    const char *tz = "Asia/Shanghai";
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
    struct tm *timeptr = localtime(&gTime);
    if (!timeptr) {
        EXPECT_PTRNE("strftime_0700", timeptr, NULL);
        return;
    }
    char buffer[gBufferSize];
    size_t count = strftime(buffer, sizeof(buffer) - 1, "%v", timeptr);
    EXPECT_TRUE("strftime_0700", count > 0);
    EXPECT_STREQ("strftime_0700", buffer, "30-Jul-2022");
}

int main(void)
{
    strftime_0100();
    strftime_0200();
    strftime_0300();
    strftime_0400();
    strftime_0500();
    strftime_0600();
    strftime_0700();
    return t_status;
}