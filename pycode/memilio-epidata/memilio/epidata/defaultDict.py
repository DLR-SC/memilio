#############################################################################
# Copyright (C) 2020-2021 German Aerospace Center (DLR-SC)
#
# Authors: Kathrin Rack, Wadim Koslow, Martin J. Kuehn
#
# Contact: Martin J. Kuehn <Martin.Kuehn@DLR.de>
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#############################################################################
"""
@file defaultDict.py

@brief Defines global parameter, lists and dictionaries

Dictionaries define values which are globally used and should be everywhere the same
- defaultDict defined default values for the function parameter

This dictionary ensures that in case of calling the functions
and of calling the console scripts the default values are the same
"""

import os
from datetime import date

default_file_path = os.path.dirname(os.path.abspath(__file__))
dfp_vec = default_file_path.split('memilio')
if len(dfp_vec) > 0:
    default_file_path = os.path.join(
        dfp_vec[0], os.path.join('memilio', 'data/pydata'))

defaultDict = {
    'read_data': False,
    'make_plot': False,
    'out_folder': default_file_path,
    'start_date': date(2020, 4, 24),
    'end_date': date.today(),
    'split_berlin': False,
    'impute_dates': False,
    'moving_average': 0,
    'file_format': 'json_timeasstring',
    'no_raw': False,
    'rep_date': False,
    'sanitize_data': 1
}

# The following dict EngEng makes sure that for all
# languages and sources the same names are used
# Rules for keys: start with small letter,
# one word, if several words start with capital letter
# do not use underscore
# Do not change the keys of this dictionary!!!!
EngEng = {
    'gender': 'Gender',
    'confirmed': 'Confirmed',
    'confirmedTotal': 'Confirmed_total',
    'confirmedPcr': 'Confirmed_PCR',
    'confirmedAb': 'Confirmed_AB',
    'recovered': 'Recovered',
    'deaths': 'Deaths',
    'idState': 'ID_State',
    'state': 'State',
    'idCounty': 'ID_County',
    'county': 'County',
    'ageRKI': 'Age_RKI',
    'age10': 'Age',
    'unknown': 'unknown',
    'female': 'female',
    'male': 'male',
    'date': 'Date',
    'hospitalized': 'Hospitalized',
    'intensive care unit': 'ICU',
    'ICU': 'ICU',
    '80+': '80+',
    '90+': '90+',
    'both': 'both',
    'all': 'all',
    'occupied_ICU': 'occupied_ICU',
    'free_ICU': 'free_ICU',
    'reporting_hospitals': 'reporting_hospitals',
    'ICU_ventilated': 'ICU_ventilated',
    'population': 'Population',
    'area': 'Area',
    'number': 'Number',
    'vaccPartial': "Vacc_partially",
    'vaccComplete': "Vacc_completed",
    'vaccRefresh': "Vacc_refreshed",
    'vaccNotComplete': "Vacc_not_completed",
    # test data
    'positiveRate': 'Positive_rate',
    # NPI data
    'npiCode': 'NPI_code',
    # mobility data refs
    'nuts3': 'NUTS3',
    'total_volume': 'Unique_trips',
    'region_name': 'County',
    'region_id': 'ID_County'
}

GerEng = {
    'Geschlecht': EngEng['gender'],
    'AnzahlFall': EngEng['confirmed'],
    'AnzahlGenesen': EngEng['recovered'],
    'AnzahlTodesfall': EngEng['deaths'],
    'IdBundesland': EngEng['idState'],
    'Bundesland': EngEng['state'],
    'IdLandkreis': EngEng['idCounty'],
    'Landkreis': EngEng['county'],
    'Altersgruppe': EngEng['ageRKI'],
    'unbekannt': EngEng['unknown'],
    'W': EngEng['female'],
    'M': EngEng['male'],
    'bundesland': EngEng['idState'],
    'betten_belegt': EngEng['occupied_ICU'],
    'betten_frei': EngEng['free_ICU'],
    'daten_stand': EngEng['date'],
    'date': EngEng['date'],
    'gemeindeschluessel': EngEng['idCounty'],
    'anzahl_standorte': EngEng['reporting_hospitals'],
    'faelle_covid_aktuell': EngEng['intensive care unit'],
    # column name new (from March 31, 2021): "faelle_covid_aktuell_invasiv_beatmet"
    'faelle_covid_aktuell_invasiv_beatmet': EngEng['ICU_ventilated'],
    'LAN_ew_GEN': EngEng['state'],
    'LAN_ew_EWZ': EngEng['population'],
    'LAN_ew_RS': EngEng['idState'],
    'EWZ': EngEng['population'],
    'GEN': EngEng['county'],
    'RS': EngEng['idCounty'],
    'Impfdatum': EngEng['date'],
    'LandkreisId_Impfort': EngEng['idCounty'],
    'Altersgruppe': EngEng['ageRKI'],
    'Anzahl': EngEng['number'],
    'Positivenanteil (%)': EngEng['positiveRate'],
    'Anteil positiv': EngEng['positiveRate'],
    'ags2': EngEng['idState'],
    'kreis': EngEng['county'],
    'ags5': EngEng['idCounty'],
    'm_code': EngEng['npiCode'],
    'code': EngEng['npiCode'], 
    'Bundesland_Id': EngEng['idState']
}

EsEng = {'fecha': EngEng['date'],
         'rango_edad': EngEng['age10'],
         'sexo': EngEng['gender'],
         'casos_confirmados': EngEng['confirmed'],
         'fallecidos': EngEng['deaths'],
         'ingresos_uci': EngEng['intensive care unit'],
         'Fecha': EngEng['date'],
         'Casos': EngEng['confirmedTotal'],
         'PCR+': EngEng['confirmedPcr'],
         'TestAc+': EngEng['confirmedAb'],
         'hospitalizados': EngEng['hospitalized'],
         'Hospitalizados': EngEng['hospitalized'],
         'cod_ine': EngEng['idState'],
         'CCAA': EngEng['state'],
         'UCI': EngEng['intensive care unit'],
         'Fallecidos': EngEng['deaths'],
         'Recuperados': EngEng['recovered'],
         'Andalucía': 'Andalucia',
         'Castilla y León': 'Castilla y Leon',
         'Cataluña': 'Cataluna',
         'País Vasco': 'Pais Vasco',
         'Aragón': 'Aragon',
         }

State = {
    1: 'Schleswig-Holstein',
    2: 'Hamburg',
    3: 'Niedersachsen',
    4: 'Bremen',
    5: 'Nordrhein-Westfalen',
    6: 'Hessen',
    7: 'Rheinland-Pfalz',
    8: 'Baden-Württemberg',
    9: 'Bayern',
    10: 'Saarland',
    11: 'Berlin',
    12: 'Brandenburg',
    13: 'Mecklenburg-Vorpommern',
    14: 'Sachsen',
    15: 'Sachsen-Anhalt',
    16: 'Thüringen',
}

County = {
    1001: 'Flensburg, Stadt',
    1002: 'Kiel, Landeshauptstadt',
    1003: 'Lübeck, Hansestadt',
    1004: 'Neumünster, Stadt',
    1051: 'Dithmarschen',
    1053: 'Herzogtum Lauenburg',
    1054: 'Nordfriesland',
    1055: 'Ostholstein',
    1056: 'Pinneberg',
    1057: 'Plön',
    1058: 'Rendsburg-Eckernförde',
    1059: 'Schleswig-Flensburg',
    1060: 'Segeberg',
    1061: 'Steinburg',
    1062: 'Stormarn',
    2000: 'Hamburg, Freie und Hansestadt',
    3101: 'Braunschweig, Stadt',
    3102: 'Salzgitter, Stadt',
    3103: 'Wolfsburg, Stadt',
    3151: 'Gifhorn',
    3153: 'Goslar',
    3154: 'Helmstedt',
    3155: 'Northeim',
    3157: 'Peine',
    3158: 'Wolfenbüttel',
    3159: 'Göttingen',
    3241: 'Region Hannover',
    3251: 'Diepholz',
    3252: 'Hameln-Pyrmont',
    3254: 'Hildesheim',
    3255: 'Holzminden',
    3256: 'Nienburg (Weser)',
    3257: 'Schaumburg',
    3351: 'Celle',
    3352: 'Cuxhaven',
    3353: 'Harburg',
    3354: 'Lüchow-Dannenberg',
    3355: 'Lüneburg',
    3356: 'Osterholz',
    3357: 'Rotenburg (Wümme)',
    3358: 'Heidekreis',
    3359: 'Stade',
    3360: 'Uelzen',
    3361: 'Verden',
    3401: 'Delmenhorst, Stadt',
    3402: 'Emden, Stadt',
    3403: 'Oldenburg (Oldenburg), Stadt',
    3404: 'Osnabrück, Stadt',
    3405: 'Wilhelmshaven, Stadt',
    3451: 'Ammerland',
    3452: 'Aurich',
    3453: 'Cloppenburg',
    3454: 'Emsland',
    3455: 'Friesland',
    3456: 'Grafschaft Bentheim',
    3457: 'Leer',
    3458: 'Oldenburg',
    3459: 'Osnabrück',
    3460: 'Vechta',
    3461: 'Wesermarsch',
    3462: 'Wittmund',
    4011: 'Bremen, Stadt',
    4012: 'Bremerhaven, Stadt',
    5111: 'Düsseldorf, Stadt',
    5112: 'Duisburg, Stadt',
    5113: 'Essen, Stadt',
    5114: 'Krefeld, Stadt',
    5116: 'Mönchengladbach, Stadt',
    5117: 'Mülheim an der Ruhr, Stadt',
    5119: 'Oberhausen, Stadt',
    5120: 'Remscheid, Stadt',
    5122: 'Solingen, Klingenstadt',
    5124: 'Wuppertal, Stadt',
    5154: 'Kleve',
    5158: 'Mettmann',
    5162: 'Rhein-Kreis Neuss',
    5166: 'Viersen',
    5170: 'Wesel',
    5314: 'Bonn, Stadt',
    5315: 'Köln, Stadt',
    5316: 'Leverkusen, Stadt',
    5334: 'Städteregion Aachen',
    5358: 'Düren',
    5362: 'Rhein-Erft-Kreis',
    5366: 'Euskirchen',
    5370: 'Heinsberg',
    5374: 'Oberbergischer Kreis',
    5378: 'Rheinisch-Bergischer Kreis',
    5382: 'Rhein-Sieg-Kreis',
    5512: 'Bottrop, Stadt',
    5513: 'Gelsenkirchen, Stadt',
    5515: 'Münster, Stadt',
    5554: 'Borken',
    5558: 'Coesfeld',
    5562: 'Recklinghausen',
    5566: 'Steinfurt',
    5570: 'Warendorf',
    5711: 'Bielefeld, Stadt',
    5754: 'Gütersloh',
    5758: 'Herford',
    5762: 'Höxter',
    5766: 'Lippe',
    5770: 'Minden-Lübbecke',
    5774: 'Paderborn',
    5911: 'Bochum, Stadt',
    5913: 'Dortmund, Stadt',
    5914: 'Hagen, Stadt der FernUniversität',
    5915: 'Hamm, Stadt',
    5916: 'Herne, Stadt',
    5954: 'Ennepe-Ruhr-Kreis',
    5958: 'Hochsauerlandkreis',
    5962: 'Märkischer Kreis',
    5966: 'Olpe',
    5970: 'Siegen-Wittgenstein',
    5974: 'Soest',
    5978: 'Unna',
    6411: 'Darmstadt, Wissenschaftsstadt',
    6412: 'Frankfurt am Main, Stadt',
    6413: 'Offenbach am Main, Stadt',
    6414: 'Wiesbaden, Landeshauptstadt',
    6431: 'Bergstraße',
    6432: 'Darmstadt-Dieburg',
    6433: 'Groß-Gerau',
    6434: 'Hochtaunuskreis',
    6435: 'Main-Kinzig-Kreis',
    6436: 'Main-Taunus-Kreis',
    6437: 'Odenwaldkreis',
    6438: 'Offenbach',
    6439: 'Rheingau-Taunus-Kreis',
    6440: 'Wetteraukreis',
    6531: 'Gießen',
    6532: 'Lahn-Dill-Kreis',
    6533: 'Limburg-Weilburg',
    6534: 'Marburg-Biedenkopf',
    6535: 'Vogelsbergkreis',
    6611: 'Kassel, documenta-Stadt',
    6631: 'Fulda',
    6632: 'Hersfeld-Rotenburg',
    6633: 'Kassel',
    6634: 'Schwalm-Eder-Kreis',
    6635: 'Waldeck-Frankenberg',
    6636: 'Werra-Meißner-Kreis',
    7111: 'Koblenz, kreisfreie Stadt',
    7131: 'Ahrweiler',
    7132: 'Altenkirchen (Westerwald)',
    7133: 'Bad Kreuznach',
    7134: 'Birkenfeld',
    7135: 'Cochem-Zell',
    7137: 'Mayen-Koblenz',
    7138: 'Neuwied',
    7140: 'Rhein-Hunsrück-Kreis',
    7141: 'Rhein-Lahn-Kreis',
    7143: 'Westerwaldkreis',
    7211: 'Trier, kreisfreie Stadt',
    7231: 'Bernkastel-Wittlich',
    7232: 'Eifelkreis Bitburg-Prüm',
    7233: 'Vulkaneifel',
    7235: 'Trier-Saarburg',
    7311: 'Frankenthal (Pfalz), kreisfreie Stadt',
    7312: 'Kaiserslautern, kreisfreie Stadt',
    7313: 'Landau in der Pfalz, kreisfreie Stadt',
    7314: 'Ludwigshafen am Rhein, kreisfreie Stadt',
    7315: 'Mainz, kreisfreie Stadt',
    7316: 'Neustadt an der Weinstraße, kreisfreie Stadt',
    7317: 'Pirmasens, kreisfreie Stadt',
    7318: 'Speyer, kreisfreie Stadt',
    7319: 'Worms, kreisfreie Stadt',
    7320: 'Zweibrücken, kreisfreie Stadt',
    7331: 'Alzey-Worms',
    7332: 'Bad Dürkheim',
    7333: 'Donnersbergkreis',
    7334: 'Germersheim',
    7335: 'Kaiserslautern',
    7336: 'Kusel',
    7337: 'Südliche Weinstraße',
    7338: 'Rhein-Pfalz-Kreis',
    7339: 'Mainz-Bingen',
    7340: 'Südwestpfalz',
    8111: 'Stuttgart, Stadtkreis',
    8115: 'Böblingen',
    8116: 'Esslingen',
    8117: 'Göppingen',
    8118: 'Ludwigsburg',
    8119: 'Rems-Murr-Kreis',
    8121: 'Heilbronn, Stadtkreis',
    8125: 'Heilbronn',
    8126: 'Hohenlohekreis',
    8127: 'Schwäbisch Hall',
    8128: 'Main-Tauber-Kreis',
    8135: 'Heidenheim',
    8136: 'Ostalbkreis',
    8211: 'Baden-Baden, Stadtkreis',
    8212: 'Karlsruhe, Stadtkreis',
    8215: 'Karlsruhe',
    8216: 'Rastatt',
    8221: 'Heidelberg, Stadtkreis',
    8222: 'Mannheim, Stadtkreis',
    8225: 'Neckar-Odenwald-Kreis',
    8226: 'Rhein-Neckar-Kreis',
    8231: 'Pforzheim, Stadtkreis',
    8235: 'Calw',
    8236: 'Enzkreis',
    8237: 'Freudenstadt',
    8311: 'Freiburg im Breisgau, Stadtkreis',
    8315: 'Breisgau-Hochschwarzwald',
    8316: 'Emmendingen',
    8317: 'Ortenaukreis',
    8325: 'Rottweil',
    8326: 'Schwarzwald-Baar-Kreis',
    8327: 'Tuttlingen',
    8335: 'Konstanz',
    8336: 'Lörrach',
    8337: 'Waldshut',
    8415: 'Reutlingen',
    8416: 'Tübingen',
    8417: 'Zollernalbkreis',
    8421: 'Ulm, Stadtkreis',
    8425: 'Alb-Donau-Kreis',
    8426: 'Biberach',
    8435: 'Bodenseekreis',
    8436: 'Ravensburg',
    8437: 'Sigmaringen',
    9161: 'Ingolstadt',
    9162: 'München, Landeshauptstadt',
    9163: 'Rosenheim, Stadtkreis',
    9171: 'Altötting',
    9172: 'Berchtesgadener Land',
    9173: 'Bad Tölz-Wolfratshausen',
    9174: 'Dachau',
    9175: 'Ebersberg',
    9176: 'Eichstätt',
    9177: 'Erding',
    9178: 'Freising',
    9179: 'Fürstenfeldbruck',
    9180: 'Garmisch-Partenkirchen',
    9181: 'Landsberg am Lech',
    9182: 'Miesbach',
    9183: 'Mühldorf a.Inn',
    9184: 'München',
    9185: 'Neuburg-Schrobenhausen',
    9186: 'Pfaffenhofen a.d.Ilm',
    9187: 'Rosenheim',
    9188: 'Starnberg',
    9189: 'Traunstein',
    9190: 'Weilheim-Schongau',
    9261: 'Landshut, Stadtkreis',
    9262: 'Passau, Stadtkreis',
    9263: 'Straubing',
    9271: 'Deggendorf',
    9272: 'Freyung-Grafenau',
    9273: 'Kelheim',
    9274: 'Landshut',
    9275: 'Passau',
    9276: 'Regen',
    9277: 'Rottal-Inn',
    9278: 'Straubing-Bogen',
    9279: 'Dingolfing-Landau',
    9361: 'Amberg',
    9362: 'Regensburg, Stadtkreis',
    9363: 'Weiden i.d.OPf.',
    9371: 'Amberg-Sulzbach',
    9372: 'Cham',
    9373: 'Neumarkt i.d.OPf.',
    9374: 'Neustadt a.d.Waldnaab',
    9375: 'Regensburg',
    9376: 'Schwandorf',
    9377: 'Tirschenreuth',
    9461: 'Bamberg, Stadtkreis',
    9462: 'Bayreuth, Stadtkreis',
    9463: 'Coburg, Stadtkreis',
    9464: 'Hof, Stadtkreis',
    9471: 'Bamberg',
    9472: 'Bayreuth',
    9473: 'Coburg',
    9474: 'Forchheim',
    9475: 'Hof',
    9476: 'Kronach',
    9477: 'Kulmbach',
    9478: 'Lichtenfels',
    9479: 'Wunsiedel i.Fichtelgebirge',
    9561: 'Ansbach, Stadtkreis',
    9562: 'Erlangen',
    9563: 'Fürth, Stadtkreis',
    9564: 'Nürnberg',
    9565: 'Schwabach',
    9571: 'Ansbach',
    9572: 'Erlangen-Höchstadt',
    9573: 'Fürth',
    9574: 'Nürnberger Land',
    9575: 'Neustadt a.d.Aisch-Bad Windsheim',
    9576: 'Roth',
    9577: 'Weißenburg-Gunzenhausen',
    9661: 'Aschaffenburg, Stadtkreis',
    9662: 'Schweinfurt, Stadtkreis',
    9663: 'Würzburg, Stadtkreis',
    9671: 'Aschaffenburg',
    9672: 'Bad Kissingen',
    9673: 'Rhön-Grabfeld',
    9674: 'Haßberge',
    9675: 'Kitzingen',
    9676: 'Miltenberg',
    9677: 'Main-Spessart',
    9678: 'Schweinfurt',
    9679: 'Würzburg',
    9761: 'Augsburg, Stadtkreis',
    9762: 'Kaufbeuren',
    9763: 'Kempten (Allgäu)',
    9764: 'Memmingen',
    9771: 'Aichach-Friedberg',
    9772: 'Augsburg',
    9773: 'Dillingen a.d.Donau',
    9774: 'Günzburg',
    9775: 'Neu-Ulm',
    9776: 'Lindau (Bodensee)',
    9777: 'Ostallgäu',
    9778: 'Unterallgäu',
    9779: 'Donau-Ries',
    9780: 'Oberallgäu',
    10041: 'Regionalverband Saarbrücken',
    10042: 'Merzig-Wadern',
    10043: 'Neunkirchen',
    10044: 'Saarlouis',
    10045: 'Saarpfalz-Kreis',
    10046: 'St. Wendel',
    11000: 'Berlin',
    11001: 'Berlin Mitte',
    11002: 'Berlin Friedrichshain-Kreuzberg',
    11003: 'Berlin Pankow',
    11004: 'Berlin Charlottenburg-Wilmersdorf',
    11005: 'Berlin Spandau',
    11006: 'Berlin Steglitz-Zehlendorf',
    11007: 'Berlin Tempelhof-Schöneberg',
    11008: 'Berlin Neukölln',
    11009: 'Berlin Treptow-Köpenick',
    11010: 'Berlin Marzahn-Hellersdorf',
    11011: 'Berlin Lichtenberg',
    11012: 'Berlin Reinickendorf',
    12051: 'Brandenburg an der Havel, Stadt',
    12052: 'Cottbus, Stadt',
    12053: 'Frankfurt (Oder), Stadt',
    12054: 'Potsdam, Stadt',
    12060: 'Barnim',
    12061: 'Dahme-Spreewald',
    12062: 'Elbe-Elster',
    12063: 'Havelland',
    12064: 'Märkisch-Oderland',
    12065: 'Oberhavel',
    12066: 'Oberspreewald-Lausitz',
    12067: 'Oder-Spree',
    12068: 'Ostprignitz-Ruppin',
    12069: 'Potsdam-Mittelmark',
    12070: 'Prignitz',
    12071: 'Spree-Neiße',
    12072: 'Teltow-Fläming',
    12073: 'Uckermark',
    13003: 'Rostock',
    13004: 'Schwerin',
    13071: 'Mecklenburgische Seenplatte',
    13072: 'Landkreis Rostock',
    13073: 'Vorpommern-Rügen',
    13074: 'Nordwestmecklenburg',
    13075: 'Vorpommern-Greifswald',
    13076: 'Ludwigslust-Parchim',
    14511: 'Chemnitz, Stadt',
    14521: 'Erzgebirgskreis',
    14522: 'Mittelsachsen',
    14523: 'Vogtlandkreis',
    14524: 'Zwickau',
    14612: 'Dresden, Stadt',
    14625: 'Bautzen',
    14626: 'Görlitz',
    14627: 'Meißen',
    14628: 'Sächsische Schweiz-Osterzgebirge',
    14713: 'Leipzig, Stadt',
    14729: 'Leipzig',
    14730: 'Nordsachsen',
    15001: 'Dessau-Roßlau, Stadt',
    15002: 'Halle (Saale), Stadt',
    15003: 'Magdeburg, Landeshauptstadt',
    15081: 'Altmarkkreis Salzwedel',
    15082: 'Anhalt-Bitterfeld',
    15083: 'Börde',
    15084: 'Burgenlandkreis',
    15085: 'Harz',
    15086: 'Jerichower Land',
    15087: 'Mansfeld-Südharz',
    15088: 'Saalekreis',
    15089: 'Salzlandkreis',
    15090: 'Stendal',
    15091: 'Wittenberg',
    16051: 'Erfurt, Stadt',
    16052: 'Gera, Stadt',
    16053: 'Jena, Stadt',
    16054: 'Suhl, Stadt',
    16055: 'Weimar, Stadt',
    16056: 'Eisenach, Stadt',
    16061: 'Eichsfeld',
    16062: 'Nordhausen',
    16063: 'Wartburgkreis',
    16064: 'Unstrut-Hainich-Kreis',
    16065: 'Kyffhäuserkreis',
    16066: 'Schmalkalden-Meiningen',
    16067: 'Gotha',
    16068: 'Sömmerda',
    16069: 'Hildburghausen',
    16070: 'Ilm-Kreis',
    16071: 'Weimarer Land',
    16072: 'Sonneberg',
    16073: 'Saalfeld-Rudolstadt',
    16074: 'Saale-Holzland-Kreis',
    16075: 'Saale-Orla-Kreis',
    16076: 'Greiz',
    16077: 'Altenburger Land'
}

IntermediateRegions = {
    0: 'Berlin',
    1: 'BielefeldPaderborn',
    2: 'Bremen',
    3: 'Chemnitz',
    4: 'Dresden',
    5: 'DuesseldorfRuhr',
    6: 'Erfurt',
    7: 'Frankfurt',
    8: 'Freiburg',
    9: 'GreifswaldStralsund',
    10: 'Göttingen',
    11: 'Hamburg',
    12: 'Hannover',
    13: 'KarlsruheMannheim',
    14: 'Kassel',
    15: 'Konstanz',
    16: 'Köln',
    17: 'Leipzig',
    18: 'Magdeburg',
    19: 'MuensterOsnabrueck',
    20: 'München',
    21: 'Neubrandenburg',
    22: 'Nuernberg',
    23: 'Passau',
    24: 'Ravensburg',
    25: 'Regensburg',
    26: 'Rostock',
    27: 'Saarbruecken',
    28: 'SchweinfurtWuerzburg',
    29: 'Siegen',
    30: 'Stuttgart',
    31: 'Trier',
    32: 'Ulm',
    33: 'VillingenSchwenningen'
}

IntermediateRegionIDsToCountyIDs = {
    0: [11000, 12051, 12053, 12054, 12060, 12061, 12063, 12064, 12065, 12067, 12068, 12069, 12070, 12072, 12073],
    1: [3255, 5711, 5754, 5758, 5762, 5766, 5770, 5774],
    2: [3251, 3352, 3356, 3357, 3361, 3401, 3402, 3403, 3405, 3451, 3452, 3453, 3455, 3457, 3458, 3461, 3462, 4011, 4012],
    3: [9464, 9475, 14511, 14521, 14522, 14523, 14524],
    4: [12052, 12062, 12066, 12071, 14612, 14625, 14626, 14627, 14628],
    5: [5111, 5112, 5113, 5114, 5116, 5117, 5119, 5120, 5122, 5124, 5154, 5158, 5162, 5166, 5170, 5370, 5512, 5513, 5562, 5911, 5913, 5914, 5915, 5916, 5954, 5958, 5962, 5974, 5978],
    6: [16051, 16052, 16053, 16054, 16055, 16056, 16063, 16064, 16065, 16066, 16067, 16068, 16069, 16070, 16071, 16073, 16074, 16075, 16076],
    7: [6411, 6412, 6413, 6414, 6432, 6433, 6434, 6435, 6436, 6437, 6438, 6439, 6440, 6531, 6532, 6533, 6534, 6535, 6631, 7133, 7134, 7141, 7315, 7331, 7339, 9661, 9671, 9676],
    8: [8311, 8315, 8316, 8317, 8336, 8337],
    9: [13073, 13075],
    10: [3155, 3159, 16061, 16062],
    11: [1001, 1002, 1003, 1004, 1051, 1053, 1054, 1055, 1056, 1057, 1058, 1059, 1060, 1061, 1062, 2000, 3353, 3354, 3355, 3359, 3360, 13004, 13074, 13076],
    12: [3101, 3102, 3103, 3151, 3153, 3154, 3157, 3158, 3241, 3252, 3254, 3256, 3257, 3351, 3358],
    13: [6431, 7311, 7313, 7314, 7316, 7318, 7319, 7332, 7333, 7334, 7337, 7338, 8211, 8212, 8215, 8216, 8221, 8222, 8226],
    14: [6611, 6632, 6633, 6634, 6635, 6636],
    15: [8335],
    16: [5314, 5315, 5316, 5334, 5358, 5362, 5366, 5374, 5378, 5382, 7111, 7131, 7135, 7137, 7138, 7140, 7143],
    17: [14713, 14729, 14730, 15001, 15002, 15082, 15084, 15087, 15088, 15091, 16077],
    18: [15003, 15081, 15083, 15085, 15086, 15089, 15090],
    19: [3404, 3454, 3456, 3459, 3460, 5515, 5554, 5558, 5566, 5570],
    20: [9161, 9162, 9163, 9171, 9172, 9173, 9174, 9175, 9176, 9177, 9178, 9179, 9180, 9181, 9182, 9183, 9184, 9185, 9186, 9187, 9188, 9189, 9190, 9261, 9273, 9274, 9277, 9279, 9761, 9762, 9763, 9764, 9771, 9772, 9773, 9774, 9777, 9778, 9779, 9780],
    21: [13071],
    22: [9361, 9371, 9373, 9377, 9461, 9462, 9463, 9471, 9472, 9473, 9474, 9476, 9477, 9478, 9479, 9561, 9562, 9563, 9564, 9565, 9571, 9572, 9573, 9574, 9575, 9576, 9577, 16072],
    23: [9262, 9271, 9272, 9275, 9276],
    24: [8435, 8436, 9776],
    25: [9263, 9278, 9362, 9363, 9372, 9374, 9375, 9376],
    26: [13003, 13072],
    27: [7312, 7317, 7320, 7335, 7336, 7340, 10041, 10042, 10043, 10044, 10045, 10046],
    28: [9662, 9663, 9672, 9673, 9674, 9675, 9677, 9678, 9679],
    29: [5966, 5970, 7132],
    30: [8111, 8115, 8116, 8117, 8118, 8119, 8121, 8125, 8126, 8127, 8128, 8135, 8136, 8225, 8231, 8235, 8236, 8237, 8415, 8416, 8417],
    31: [7211, 7231, 7232, 7233, 7235],
    32: [8421, 8425, 8426, 8437, 9775],
    33: [8325, 8326, 8327]
}


def invert_dict(dict_to_invert):
    """! Inverts the roles of keys and values of dictionary to create
    a new dictionary where the previous keys are the values and vice versa.

    @param dict_to_invert Dictionary.
    @return Inverted dictionary.
    """
    return dict([(val, key) for key, val in dict_to_invert.items()])
