/* -*- coding: utf-8 -*- */
/* Copyright (C) 2000-2012 by George Williams */
/*
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.

 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.

 * The name of the author may not be used to endorse or promote products
 * derived from this software without specific prior written permission.

 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdint.h>

#include "ffglib_compat.h"
#include "langfreq.h"
#include "splinefont.h"
#include "splinesaveafm.h"
#include "ustring.h"

/* ************************************************************************** */
/* ***************** Sample Text in Many Scripts/Languages ****************** */
/* ************************************************************************** */

/* English */
static const char* _simple[] = {" A quick brown fox jumps over the lazy dog.",
                                NULL};
static const char* _simplelatnchoices[] = {
    /* English */
    " A quick brown fox jumps over the lazy dog.",
    " Few zebras validate my paradox, quoth Jack Xeno",
    " A quick brown vixen jumps for the lazy dog.",
    " The quick blonde doxy jumps over an unfazed wag.",
    /* Swedish */
    "flygande bäckasiner söka hwila på mjuka tuvor",
    /* German (from http://shiar.net/misc/txt/pangram.en) */
    /* Twelve boxing fighters hunted Eva across the great dike of Sylt */
    "zwölf Boxkämpfer jagten Eva quer über den großen Sylter Deich",
    /* French (from http://shiar.net/misc/txt/pangram.en) */
    /* Carry this old wisky to the blond smoking judge */
    "portez ce vieux whisky au juge blond qui fume",
    "Les naïfs ægithales hâtifs pondant à Noël où il gèle sont sûrs d'être "
    "déçus et de voir leurs drôles d'œufs abîmés.",
    /* Dutch (from http://shiar.net/misc/txt/pangram.en) */
    /* Sexy by body, though scared by the swimsuit */
    " sexy qua lijf, doch bang voor 't zwempak",
    /* Polish (from http://shiar.net/misc/txt/pangram.en) */
    /* to push a hedgehog or eight bins of figs in this boat */
    " pchnąć w tę łódź jeża lub ośm skrzyń fig ",
    /* Slovaka (from http://shiar.net/misc/txt/pangram.en) */
    " starý kôň na hŕbe kníh žuje tíško povädnuté ruže, na stĺpe sa ďateľ učí "
    "kvákať novú ódu o živote ",
    /* Czech (from http://shiar.net/misc/txt/pangram.en) */
    " příliš žluťoučký kůň úpěl ďábelské kódy ", NULL};
static uint32_t _simplelatnlangs[] = {
    CHR('E', 'N', 'G', ' '), CHR('E', 'N', 'G', ' '), CHR('E', 'N', 'G', ' '),
    CHR('E', 'N', 'G', ' '), CHR('S', 'V', 'E', ' '), CHR('D', 'E', 'U', ' '),
    CHR('F', 'R', 'A', ' '), CHR('F', 'R', 'A', ' '), CHR('N', 'L', 'D', ' '),
    CHR('P', 'L', 'K', ' '), CHR('S', 'L', 'V', ' '), CHR('C', 'S', 'Y', ' ')};

/* Hebrew (from http://shiar.net/misc/txt/pangram.en) */
static const char* _simplehebrew[] = {
    " ?דג סקרן שט בים מאוכזב ולפתע מצא לו חברה איך הקליטה ", NULL};
/* Katakana (from http://shiar.net/misc/txt/pangram.en) */
static const char* _simplekata[] = {
    " イロハニホヘト チリヌルヲ ワカヨタレソ ツネナラム/ ウヰノオクヤマ "
    "ケフコエテ アサキユメミシ ヱヒモセスン ",
    NULL};
/* Hiragana (from http://shiar.net/misc/txt/pangram.en) */
static const char* _simplehira[] = {
    " いろはにほへとちりぬるを/ わかよたれそつねならむ/ "
    "うゐのおくやまけふこえて/ あさきゆめみしゑひもせす ",
    NULL};
/* Russian */
static const char* _simplecyrill[] = {
    " Съешь ещё этих мягких французских булок, да выпей чаю!", NULL};
static const char* _simplecyrillchoices[] = {
    /* Eat more those soft french 'little-sweet-breads' and drink tea */
    " Съешь ещё этих мягких французских булок, да выпей чаю!",
    /* "In the deep forests of South citrus lived... /answer/Yeah but
       falsificated one!" */
    " В чащах юга жил-был цитрус -- да, но фальшивый экземпляръ!",
    /* A kind lamplighter with grimy face wants to show me a stunt. */
    "Љубазни фењерџија чађавог лица хоће да ми покаже штос.",
    /* More frequent filtering through the reticular bag improves
    fertilization of genetic hybrids. */
    "Чешће цeђење мрeжастим џаком побољшава фертилизацију генских хибрида.",
    NULL};
static uint32_t _simplecyrilliclangs[] = {CHR('R', 'U', 'S', ' '), /* Russian */
                                          CHR('R', 'U', 'S', ' '),
                                          CHR('S', 'R', 'B', ' '), /* Serbian */
                                          CHR('S', 'R', 'B', ' ')};
/* Russian */
static const char* _annakarenena[] = {
    " Всѣ счастливыя семьи похожи другъ на друга, каждая несчастливая семья "
    "несчастлива по-своему.",
    " Все смѣшалось въ домѣ Облонскихъ. Жена узнала, что мужъ былъ связи съ "
    "бывшею въ ихъ домѣ француженкою-гувернанткой, и объявила мужу, что не "
    "можетъ жить съ нимъ въ одномъ домѣ.",
    NULL};
/* Serbian (Cyrillic) */
static const char* _serbcyriljohn[] = {
    "У почетку беше Реч Божија, и та Реч беше у Бога, и Бог беше Реч.", NULL};
/* Spanish */
static const char* _donquixote[] = {
    " En un lugar de la Mancha, de cuyo nombre no quiero acordarme, no ha "
    "mucho tiempo que vivía un hidalgo de los de lanza en astillero, adarga "
    "antigua, rocín flaco y galgo corredor.",
    NULL};
/* German */
static const char* _faust[] = {
    "Ihr naht euch wieder, schwankende Gestalten,",
    "Die früh sich einst dem trüben Blick gezeigt.",
    "Versuch ich wohl, euch diesmal festzuhalten?",
    "Fühl ich mein Herz noch jenem Wahn geneigt?",
    "Ihr drängt euch zu! Nun gut, so mögt ihr walten,",
    "Wie ihr aus Dunst und Nebel um mich steigt;",
    "Mein Busen fühlt sich jugendlich erschüttert",
    "Vom Zauberhauch, der euren Zug umwittert.",
    NULL};
/* Anglo Saxon */
static const char* _beorwulf[] = {"Hwæt, we Gar-Dena  in geardagum",
                                  "þeodcyninga  þrym gefrunon,",
                                  "hu ða æþelingas  ellen fremedon.",
                                  " Oft Scyld Scefing  sceaþena þreatum,",
                                  "monegum mægþum  meodosetla ofteah;",
                                  "egsode Eorle.  Syððan ærest wearð",
                                  "feasceaft funden,  (he þæs frofre gebad)",
                                  "weox under wolcnum,  weorðmyndum þah,",
                                  "oðþæt him æghwyle  þara ymbsittendra",
                                  "ofer hronrade  hyan scolde,",
                                  "gomban gyldan: þæt wæs god cyning!",
                                  NULL};
/* Italian */
static const char* _inferno[] = {" Nel mezzo del cammin di nostra vita",
                                 "mi ritrovai per una selva obscura,",
                                 "ché la diritta via era smarrita.",
                                 " Ahi quanto a dir qual era è cosa dura",
                                 "esta selva selvaggia e aspra e forte",
                                 "che nel pensier rinova la paura!",
                                 NULL};
/* Latin */
static const char* _debello[] = {
    " Gallia est omnis dīvīsa in partēs trēs, quārum ūnum incolunt Belgae, "
    "aliam Aquītānī, tertiam, quī ipsōrum linguā Celtae, nostrā Gallī "
    "appelantur. Hī omnēs linguā, īnstitūtīs, lēgibus inter sē differunt. "
    "Gallōs ab Aquītānīs Garumna flūmen, ā Belgīs Matrona et Sēquana dīvidit.",
    NULL};
/* French */
static const char* _pheadra[] = {
    "Le dessein en est pris: je pars, cher Théramène,",
    "Et quitte le séjour de l'aimable Trézène.",
    "Dans le doute mortel dont je suis agité,",
    "Je commence à rougir de mon oisiveté.",
    "Depuis plus de six mois éloigné de mon père,",
    "J'ignore le destin d'une tête si chère;",
    "J'ignore jusqu'aux lieux qui le peuvent cacher.",
    NULL};
/* Classical Greek */
static const char* _antigone[] = {"Ὦ κοινὸν αὐτάδελφον Ἰσμήνης κάρα,",
                                  "ἆῤ οἶσθ᾽ ὅτι Ζεὺς τῶν ἀπ᾽ Οἰδίπου κακῶν",
                                  "ὁποῖον οὐχὶ νῷν ἔτι ζσαιν τελεῖ;", NULL};
/* Hebrew */ /* Seder */
static const char* _hebrew[] = {
    "וְאָתָא מַלְאַךְ הַמָּוֶת, וְשָׁחַט לְשּׁוׂחֵט, רְּשָׁחַט לְתוׂרָא, רְּשָׁתַה לְמַּיָּא, דְּכָכָה לְנוּרָא, דְּשָׂרַף "
    "לְחוּטְרָא, דְּהִכָּה לְכַלְכָּא, דְּנָשַׁךְ לְשׁוּנְרָא, דְּאָכְלָה לְגַדְיָא, דִּזְבַן אַבָּא בִּתְרֵי זוּזֵי. חַד "
    "גַּדְיָא, חַד גַּדְיָא.",
    "וְאָתָא הַקָּדוֹשׁ כָּדוּךְ הוּא, וְשָׁחַט לְמַלְאַךְ הַמָּוֶת, רְּשָׁחַט לְשּׁוׂחֵט, רְּשָׁחַט לְתוׂרָא, רְּשָׁתַה לְמַּיָּא, "
    "דְּכָכָה לְנוּרָא, דְּשָׂרַף לְחוּטְרָא, דְּהִכָּה לְכַלְכָּא, דְּנָשַׁךְ לְשׁוּנְרָא, דְּאָכְלָה לְגַדְיָא, דִּזְבַן אַבָּא "
    "בִּתְרֵי זוּזֵי. חַד גַּדְיָא, חַד גַּדְיָא.",
    NULL};
/* Arabic with no dots or vowel marks */
static const char* _arabic[] = {
    "لقيت الحكمة العادلة حبا عظيما من الشعب. انسحبت من النادی فی السنة "
    "الماضية. وقعت فی الوادی فانكسرت يدی. قابلت صديقی عمرا الكاتب القدير فی "
    "السوق فقال لی انه ارسل الى الجامعة عددا من مجلته الجديدة. احتل الامير "
    "فيصل مدينة دمسق فی الحرب العالمية ودخلها راكبا على فرسه المحبوبة. ارسل "
    "عمر خالدا الى العراق ولاكن بعد مدة قصيرة وجه خالد جيشه الى سورية. قدم على "
    "دمشق واستطاع فتحها. قبل احتل عمر القدس وعقد جلستة مع حاكم القدس وقد تكلم "
    "معه عن فتح المدينة. ثم رجع عمر الى المدنة المنورة.",
    NULL};
/* Renaissance English with period ligatures */
static const char* _muchado[] = {
    " But till all graces be in one woman, one womã ſhal not com in my grace: "
    "rich ſhe ſhall be thats certain, wiſe, or ile none, vertuous, or ile "
    "neuer cheapen her.",
    NULL};
/* Modern (well, Dickens) English */
static const char* _chuzzlewit[] = {
    " As no lady or gentleman, with any claims to polite breeding, can"
    " possibly sympathize with the Chuzzlewit Family without being first"
    " assured of the extreme antiquity of the race, it is a great satisfaction"
    " to know that it undoubtedly descended in a direct line from Adam and"
    " Eve; and was, in the very earliest times, closely connected with the"
    " agricultural interest. If it should ever be urged by grudging and"
    " malicious persons, that a Chuzzlewit, in any period of the family"
    " history, displayed an overweening amount of family pride, surely the"
    " weakness will be considered not only pardonable but laudable, when the"
    " immense superiority of the house to the rest of mankind, in respect of"
    " this its ancient origin, is taken into account.",
    NULL};
/* Middle Welsh */
static const char* _mabinogion[] = {
    " Gan fod Argraffiad Rhydychen o'r Mabinogion yn rhoi'r testun yn union "
    "fel y digwydd yn y llawysgrifau, ac felly yn cyfarfod â gofyn yr "
    "ysgolhaig, bernais mai gwell mewn llawlyfr fel hwn oedd golygu peth arno "
    "er mwyn helpu'r ieuainc a'r dibrofiad yn yr hen orgraff.",
    NULL};
/* Swedish */
static const char* _PippiGarOmBord[] = {
    "Om någon människa skulle komma resande till den lilla, lilla staden och "
    "så kanske ett tu tre råka förirra sig lite för långt bort åt ena "
    "utkanten, då skulle den månniskan få se Villa Villekulla. Inte för att "
    "huset var så mycket att titta på just, en rätt fallfårdig gammal villa "
    "och en rätt vanskött gammal trädgård runt omkring, men främlingen skulle "
    "kanske i alla fall stanna och undra vem som bodde där.",
    NULL};
/* Czech */
static const char* _goodsoldier[] = {
    " „Tak nám zabili Ferdinanda,“ řekla posluhovačka panu Švejkovi, který "
    "opustiv před léty vojenskou službu, když byl definitivně prohlášen "
    "vojenskou lékařskou komisí za blba, živil se prodejem psů, ošklivých "
    "nečistokrevných oblud, kterým padělal rodokmeny.",
    " Kromě tohoto zaměstnání byl stižen revmatismem a mazal si právě kolena "
    "opodeldokem.",
    NULL};
/* Lithuanian */
static const char* _lithuanian[] = {
    " Kiekviena šventė yra surišta su praeitimi. Nešvenčiamas gimtadienis, "
    "kai, kūdikis gimsta. Ir po keliolikos metų gimtinės arba vardinės nėra "
    "tiek reikšmingos, kaip sulaukus 50 ar 75 metų. Juo tolimesnis įvykis, tuo "
    "šventė darosi svarbesnė ir iškilmingesnė.",
    NULL};
/* Polish */
static const char* _polish[] = {
    " Język prasłowiański miał w zakresie deklinacji (fleksji imiennej) "
    "następujące kategorie gramatyczne: liczby, rodzaju i przypadku. Poza tym "
    "istniały w nim (w zakresie fleksji rzeczownika) różne «odmiany», czyli "
    "typy deklinacyjne. Im dawniej w czasie, tym owe różnice deklinacyjne "
    "miały mniejszy związek z semantyką rzeczownika.",
    NULL};
/* Slovene */
static const char* _slovene[] = {
    " Razvoj glasoslovja je diametralno drugačen od razvoja morfologije.",
    " V govoru si besede slede. V vsaki sintagmi dobi beseda svojo vrednost, "
    "če je zvezana z besedo, ki je pred njo, in z besedo, ki ji sledi.",
    NULL};
/* Macedonian */
static const char* _macedonian[] = {
    " Македонскиот јазик во балканската јазична средина и наспрема соседните "
    "словенски јаеици. 1. Македонскиот јазик се говори во СР Македонија, и "
    "надвор од нејзините граници, во оние делови на Македонија што по "
    "балканските војни влегоа во составот на Грција и Бугарија.",
    NULL};
/* Bulgarian */
static const char* _bulgarian[] = {
    " ПРЕДМЕТ И ЗАДАЧИ НА ФОНЕТИКАТА Думата фонетика произлиза от гръцката "
    "дума фоне, която означава „звук“, „глас“, „тон“.",
    NULL};
/* Korean Hangul */
static const char* _hangulsijo[] = {"어버이 살아신 제 섬길 일란 다 하여라",
                                    "지나간 후면 애닯다 어이 하리",
                                    "평생에 고쳐 못할 일이 이뿐인가 하노라",
                                    "- 정철",
                                    "",
                                    "이고 진 저 늙은이 짐 벗어 나를 주오",
                                    "나는 젊었거니 돌이라 무거울까",
                                    "늙기도 설워라커든 짐을 조차 지실까",
                                    "- 정철",
                                    NULL};
/* Chinese traditional */
/* https://en.wikipedia.org/wiki/Tao_Te_Ching */
static const char* _TaoTeChing[] = {"道可道非常道，", "名可名非常名。", NULL};
/* http://gan.wikipedia.org/wiki/%E5%B0%87%E9%80%B2%E9%85%92 */
static const char* _LiBai[] = {"將進酒",
                               "",
                               "君不見 黃河之水天上來 奔流到海不復回",
                               "君不見 高堂明鏡悲白髮 朝如青絲暮成雪",
                               "人生得意須盡歡 莫使金樽空對月",
                               "天生我材必有用 千金散盡還復來",
                               "烹羊宰牛且為樂 會須一飲三百杯",
                               "岑夫子 丹丘生 將進酒 君莫停",
                               "與君歌一曲 請君為我側耳聽",
                               "鐘鼓饌玉不足貴 但願長醉不願醒",
                               "古來聖賢皆寂寞 惟有飲者留其名",
                               "陳王昔時宴平樂 斗酒十千恣讙謔",
                               "主人何為言少錢 徑須沽取對君酌",
                               "五花馬 千金裘 呼兒將出換美酒 與爾同消萬古愁",
                               NULL};
static const char* _LiBaiShort[] = {
    "將進酒", "", "君不見 黃河之水天上來 奔流到海不復回",
    "君不見 高堂明鏡悲白髮 朝如青絲暮成雪", NULL};
/* Japanese */
/* https://ja.wikipedia.org/wiki/%E6%BA%90%E6%B0%8F%E7%89%A9%E8%AA%9E */
static const char* _Genji[] = {
    "源氏物語（紫式部）：いづれの御時にか、女御・更衣あまた "
    "さぶらひたまひけるなかに、いとやむごとなき "
    "際にはあらぬが、すぐれてときめきたまふありけり。",
    NULL};
/* http://www.geocities.jp/sybrma/42souseki.neko.html */
static const char* _IAmACat[] = {"吾輩は猫である（夏目漱石）：吾輩は猫である",
                                 NULL};

/* The following translations of the gospel according to John are all from */
/*  Compendium of the world's languages. 1991 Routledge. by George L. Campbell*/

/* Belorussian */
static const char* _belorussianjohn[] = {
    "У пачатку было Слова, і Слова было ў Бога, і Богам было Слова. Яно было ў "
    "пачатку ў Бога",
    NULL};
/* basque */
static const char* _basquejohn[] = {
    "Asieran Itza ba-zan, ta Itza Yainkoagan zan, ta Itza Yainko zan.",
    "Asieran Bera Yainkoagan zan.", NULL};
/* danish */
static const char* _danishjohn[] = {
    "Begyndelsen var Ordet, og Ordet var hos Gud, og Ordet var Gud.",
    "Dette var i Begyndelsen hos Gud.", NULL};
/* dutch */
static const char* _dutchjohn[] = {
    "In den beginne was het Woord en het Woord was bij God en het Woord was "
    "God.",
    "Dit was in den beginne bij God.", NULL};
/* finnish */
static const char* _finnishjohn[] = {
    "Alussa oli Sana, ja Sana oli Jumalan luona, Sana oli Jumala.",
    "ja hä oli alussa Jumalan luona.", NULL};
/* georgian */
/* Hmm, the first 0x10e0 might be 0x10dd, 0x301 */
static const char* _georgianjohn[] = {
    "პირველითგან იყო სიტყუუ̂ა, და სიტყუუ̂ა იგი იყო ღუ̂თისა თანა, და დიერთი იყო "
    "სიტყუუ̂ა იგი.",
    "ესე იყო პირველითგან დიერთი თინი.", NULL};
/* icelandic */
static const char* _icelandicjohn[] = {
    "Í upphafi var Orðið og Orðið var hjà Guði, og Orðið var Guði.",
    "Það var í upphafi hjá Guði.", NULL};
/* irish */
static const char* _irishjohn[] = {
    "Bhí an Briathar(I) ann i dtús báire agus bhí an Briathar in éineacht le "
    "Dia, agus ba Dhia an Briathar.",
    "Bhí sé ann i dtús báire in éineacht le Dia.", NULL};
/* Bokmål norwegian */
static const char* _norwegianjohn[] = {
    "I begynnelsen var Ordet, Ordet var hos Gud, og Ordet var Gud.",
    "Han var i begynnelsen hos Gud.",
    "Alt er blitt til ved ham; uten ham er ikke noe blitt til av alt som er "
    "til.",
    NULL};
/* Nynorsk norwegian */
static const char* _nnorwegianjohn[] = {
    "I opphavet var Ordet, og Ordet var hjå Gud, og Ordet var Gud.",
    "Han var i opphavet hjå Gud.", NULL};
/* old church slavonic */
static const char* _churchjohn[] = {
    "Въ нача́лѣ бѣ̀ сло́во и҆ сло́во бѣ̀ къ бг҃ꙋ, и҆ бг҃ъ бѣ̀ сло́во.",
    "Се́й бѣ̀ и҆сконѝ къ бг҃ꙋ.", NULL};
/* swedish */
static const char* _swedishjohn[] = {
    "I begynnelsen var Ordet, och Ordet var hos Gud, och Ordet var Gud.",
    "Han var i begynnelsen hos Gud.", NULL};
/* portuguese */
static const char* _portjohn[] = {
    "No Principio era a Palavra, e a Palavra estava junto de Deos, e a Palavra "
    "era Deos.",
    "Esta estava no principio junto de Deos.", NULL};
/* cherokee */
static const char* _cherokeejohn[] = {
    "ᏗᏓᎴᏂᏯᎬ ᎧᏃᎮᏘ ᎡᎮᎢ, ᎠᎴ ᎾᏯᎩ ᎧᏃᎮᏘ ᎤᏁᎳᏅᎯ ᎢᏧᎳᎭ ᎠᏘᎮᎢ, ᎠᎴ ᎾᏯᎩ ᎧᏃᎮᏘ ᎤᏁᎳᏅᎯ ᎨᏎᎢ.",
    "ᏗᏓᎴᏂᏯᎬ ᎾᏯᎩ ᎤᏁᎳᏅᎯ ᎢᏧᎳᎭ ᎠᏘᎮᎢ", NULL};
/* swahili */
static const char* _swahilijohn[] = {
    "Hapo mwanzo kulikuwako Neno, naye Neno alikuwako kwa Mungo, naye Neno "
    "alikuwa Mungu, Huyo mwanzo alikuwako kwa Mungu.",
    "Vyote vilvanyika kwa huyo; wala pasipo yeye hakikufanyika cho chote "
    "kilichofanyiki.",
    NULL};
/* thai */ /* I'm sure I've made transcription errors here, I can't figure out
              what "0xe27, 0xe38, 0xe4d" really is */
static const char* _thaijohn[] = {"๏ ในทีเดิมนะนพวุํลอโฆเปนอยู่ แลเปนอยู่ดว้ยกันกับ พวุํเฆ้า",
                                  NULL};
/* Mayan K'iche' of Guatemala */ /* Prolog to Popol Wuj */ /* Provided by Daniel
                                                              Johnson */
static const char* _mayanPopolWuj[] = {
    "Are u xe' ojer tzij waral, C'i Che' u bi'. Waral xchikatz'ibaj-wi, "
    "xchikatiquiba-wi ojer tzij, u ticaribal, u xe'nabal puch ronojel xban pa "
    "tinamit C'i Che', ramak C'i Che' winak.",
    NULL};

/* I've omitted cornish. no interesting letters. no current speakers */

/* http://www.ethnologue.com/iso639/codes.asp */
enum scripts {
    sc_latin,
    sc_greek,
    sc_cyrillic,
    sc_georgian,
    sc_hebrew,
    sc_arabic,
    sc_hangul,
    sc_chinesetrad,
    sc_chinesemod,
    sc_kanji,
    sc_hiragana,
    sc_katakana
};
static struct langsamples {
    const char** sample;
    const char* iso_lang; /* ISO 639 two character abbreviation */
    enum scripts script;
    uint32_t otf_script, lang;
} sample[] = {
    {_simple, "various", sc_latin, CHR('l', 'a', 't', 'n'),
     CHR('E', 'N', 'G', ' ')},
    {_simplecyrill, "various", sc_cyrillic, CHR('c', 'y', 'r', 'l'),
     CHR('R', 'U', 'S', ' ')},
    {_simplehebrew, "he", sc_hebrew, CHR('h', 'e', 'b', 'r'),
     CHR('I', 'W', 'R', ' ')},
    {_simplekata, "ja", sc_katakana, CHR('k', 'a', 'n', 'a'),
     CHR('J', 'A', 'N', ' ')},
    {_simplehira, "ja", sc_hiragana, CHR('k', 'a', 'n', 'a'),
     CHR('J', 'A', 'N', ' ')},
    {_faust, "de", sc_latin, CHR('l', 'a', 't', 'n'), CHR('D', 'E', 'U', ' ')},
    {_pheadra, "fr", sc_latin, CHR('l', 'a', 't', 'n'),
     CHR('F', 'R', 'A', ' ')},
    {_antigone, "el", sc_greek, CHR('g', 'r', 'e', 'k'),
     CHR('P', 'G', 'R', ' ')}, /* Is this polytonic? */
    {_annakarenena, "ru", sc_cyrillic, CHR('c', 'y', 'r', 'l'),
     CHR('R', 'U', 'S', ' ')},
    {_serbcyriljohn, "sr", sc_cyrillic, CHR('c', 'y', 'r', 'l'),
     CHR('S', 'R', 'B', ' ')},
    {_debello, "la", sc_latin, CHR('l', 'a', 't', 'n'),
     CHR('L', 'A', 'T', ' ')},
    {_hebrew, "he", sc_hebrew, CHR('h', 'e', 'b', 'r'),
     CHR('I', 'W', 'R', ' ')},
    {_arabic, "ar", sc_arabic, CHR('a', 'r', 'a', 'b'),
     CHR('A', 'R', 'A', ' ')},
    {_hangulsijo, "ko", sc_hangul, CHR('h', 'a', 'n', 'g'),
     CHR('K', 'O', 'R', ' ')},
    {_TaoTeChing, "zh", sc_chinesetrad, CHR('h', 'a', 'n', 'i'),
     CHR('Z', 'H', 'T', ' ')},
    {_LiBai, "zh", sc_chinesetrad, CHR('h', 'a', 'n', 'i'),
     CHR('Z', 'H', 'T', ' ')},
    {_Genji, "ja", sc_kanji, CHR('h', 'a', 'n', 'i'), CHR('J', 'A', 'N', ' ')},
    {_IAmACat, "ja", sc_kanji, CHR('h', 'a', 'n', 'i'),
     CHR('J', 'A', 'N', ' ')},
    {_donquixote, "es", sc_latin, CHR('l', 'a', 't', 'n'),
     CHR('E', 'S', 'P', ' ')},
    {_inferno, "it", sc_latin, CHR('l', 'a', 't', 'n'),
     CHR('I', 'T', 'A', ' ')},
    {_beorwulf, "enm", sc_latin, CHR('l', 'a', 't', 'n'),
     CHR('E', 'N', 'G', ' ')}, /* 639-2 name for middle english */
    {_muchado, "eng", sc_latin, CHR('l', 'a', 't', 'n'),
     CHR('E', 'N', 'G', ' ')}, /* 639-2 name for modern english */
    {_chuzzlewit, "en", sc_latin, CHR('l', 'a', 't', 'n'),
     CHR('E', 'N', 'G', ' ')}, /* 639-2 name for modern english */
    {_PippiGarOmBord, "sv", sc_latin, CHR('l', 'a', 't', 'n'),
     CHR('S', 'V', 'E', ' ')},
    {_mabinogion, "cy", sc_latin, CHR('l', 'a', 't', 'n'),
     CHR('W', 'E', 'L', ' ')},
    {_goodsoldier, "cs", sc_latin, CHR('l', 'a', 't', 'n'),
     CHR('C', 'S', 'Y', ' ')},
    {_macedonian, "mk", sc_cyrillic, CHR('c', 'y', 'r', 'l'),
     CHR('M', 'K', 'D', ' ')},
    {_bulgarian, "bg", sc_cyrillic, CHR('c', 'y', 'r', 'l'),
     CHR('B', 'G', 'R', ' ')},
    {_belorussianjohn, "be", sc_cyrillic, CHR('c', 'y', 'r', 'l'),
     CHR('B', 'E', 'L', ' ')},
    {_churchjohn, "cu", sc_cyrillic, CHR('c', 'y', 'r', 'l'),
     CHR('C', 'S', 'L', ' ')},
    {_lithuanian, "lt", sc_latin, CHR('l', 'a', 't', 'n'),
     CHR('L', 'T', 'H', ' ')},
    {_polish, "pl", sc_latin, CHR('l', 'a', 't', 'n'), CHR('P', 'L', 'K', ' ')},
    {_slovene, "sl", sc_latin, CHR('l', 'a', 't', 'n'),
     CHR('S', 'L', 'V', ' ')},
    {_irishjohn, "ga", sc_latin, CHR('l', 'a', 't', 'n'),
     CHR('I', 'R', 'I', ' ')},
    {_basquejohn, "eu", sc_latin, CHR('l', 'a', 't', 'n'),
     CHR('E', 'U', 'Q', ' ')},
    {_portjohn, "pt", sc_latin, CHR('l', 'a', 't', 'n'),
     CHR('P', 'T', 'G', ' ')},
    {_icelandicjohn, "is", sc_latin, CHR('l', 'a', 't', 'n'),
     CHR('I', 'S', 'L', ' ')},
    {_danishjohn, "da", sc_latin, CHR('l', 'a', 't', 'n'),
     CHR('D', 'A', 'N', ' ')},
    {_swedishjohn, "sv", sc_latin, CHR('l', 'a', 't', 'n'),
     CHR('S', 'V', 'E', ' ')},
    {_norwegianjohn, "no", sc_latin, CHR('l', 'a', 't', 'n'),
     CHR('N', 'O', 'R', ' ')},
    {_nnorwegianjohn, "no", sc_latin, CHR('l', 'a', 't', 'n'),
     CHR('N', 'O', 'R', ' ')},
    {_dutchjohn, "nl", sc_latin, CHR('l', 'a', 't', 'n'),
     CHR('N', 'L', 'D', ' ')},
    {_finnishjohn, "fi", sc_latin, CHR('l', 'a', 't', 'n'),
     CHR('F', 'I', 'N', ' ')},
    {_cherokeejohn, "chr", sc_latin, CHR('l', 'a', 't', 'n'),
     CHR('C', 'H', 'R', ' ')},
    {_thaijohn, "th", sc_latin, CHR('l', 'a', 't', 'n'),
     CHR('T', 'H', 'A', ' ')},
    {_georgianjohn, "ka", sc_georgian, CHR('g', 'e', 'o', 'r'),
     CHR('K', 'A', 'T', ' ')},
    {_swahilijohn, "sw", sc_latin, CHR('l', 'a', 't', 'n'),
     CHR('S', 'W', 'K', ' ')},
    {_mayanPopolWuj, "QUT", sc_latin, CHR('l', 'a', 't', 'n'),
     CHR('Q', 'U', 'T', ' ')},
    {NULL, NULL, sc_latin, 0, 0}};

static void OrderSampleByLang(void) {
    const char* lang = getenv("LANG");
    char langbuf[12], *pt;
    int i, j;
    int simple_pos;

    if (lang == NULL) return;

    strncpy(langbuf, lang, 10);
    langbuf[10] = '\0';
    for (j = 0; j < 3; ++j) {
        if (j == 1) {
            for (pt = langbuf; *pt != '\0' && *pt != '.'; ++pt);
            *pt = '\0';
        } else if (j == 2) {
            for (pt = langbuf; *pt != '\0' && *pt != '_'; ++pt);
            *pt = '\0';
        }
        for (i = 0; sample[i].sample != NULL; ++i)
            if (strcmp(sample[i].iso_lang, langbuf) == 0) {
                struct langsamples temp;
                temp = sample[i];
                sample[i] = sample[2];
                sample[2] = temp;
                goto out;
            }
    }
out:
    simple_pos = 0;
    if (strcmp(langbuf, "sv") == 0)
        simple_pos = 4;
    else if (strcmp(langbuf, "de") == 0)
        simple_pos = 5;
    else if (strcmp(langbuf, "fr") == 0)
        simple_pos = 6 + ff_random_int_range(0, 2);
    else if (strcmp(langbuf, "nl") == 0)
        simple_pos = 8;
    else if (strcmp(langbuf, "pl") == 0)
        simple_pos = 9;
    else if (strcmp(langbuf, "sl") == 0)
        simple_pos = 10;
    else if (strcmp(langbuf, "cs") == 0)
        simple_pos = 11;
    else
        simple_pos = ff_random_int_range(0, 4);
    _simple[0] = _simplelatnchoices[simple_pos];
    sample[0].lang = _simplelatnlangs[simple_pos];

    for (j = 0; _simplecyrillchoices[j] != NULL; ++j);
    simple_pos = ff_random_int_range(0, j + 1);
    _simplecyrill[0] = _simplecyrillchoices[simple_pos];
    sample[1].lang = _simplecyrilliclangs[simple_pos];
}

static int AllChars(SplineFont* sf, const char* str) {
    int i, ch;
    SplineChar* sc;
    struct altuni* alt;

    if (sf->subfontcnt == 0) {
        while ((ch = utf8_ildb(&str)) != '\0') {
            for (i = 0; i < sf->glyphcnt; ++i)
                if ((sc = sf->glyphs[i]) != NULL) {
                    if (sc->unicodeenc == ch) break;
                    for (alt = sc->altuni; alt != NULL; alt = alt->next)
                        if (alt->vs == -1 && alt->unienc == ch) break;
                    if (alt != NULL) break;
                }
            if (i == sf->glyphcnt || !SCWorthOutputting(sf->glyphs[i]))
                return (false);
        }
    } else {
        int max = 0, j;
        for (i = 0; i < sf->subfontcnt; ++i)
            if (sf->subfonts[i]->glyphcnt > max)
                max = sf->subfonts[i]->glyphcnt;
        while ((ch = utf8_ildb(&str)) != '\0') {
            for (i = 0; i < max; ++i) {
                for (j = 0; j < sf->subfontcnt; ++j)
                    if (i < sf->subfonts[j]->glyphcnt &&
                        sf->subfonts[j]->glyphs[i] != NULL)
                        break;
                if (j != sf->subfontcnt)
                    if (sf->subfonts[j]->glyphs[i]->unicodeenc == ch) break;
            }
            if (i == max || !SCWorthOutputting(sf->subfonts[j]->glyphs[i]))
                return (false);
        }
    }
    return (true);
}

static int ScriptInList(uint32_t script, uint32_t* scripts, int scnt) {
    int s;

    for (s = 0; s < scnt; ++s)
        if (script == scripts[s]) return (true);

    return (false);
}

extern "C" unichar_t* PrtBuildDef(SplineFont* sf, void* tf,
                                  void (*langsyscallback)(void* tf, int end,
                                                          uint32_t script,
                                                          uint32_t lang)) {
    int i, j, gotem, len, any = 0, foundsomething = 0;
    unichar_t* ret = NULL;
    const char** cur;
    uint32_t scriptsdone[100], scriptsthere[100], langs[100];
    char* randoms[100];
    char buffer[220], *pt;
    int scnt, s, therecnt, rcnt;

    OrderSampleByLang();
    therecnt = SF2Scripts(sf, scriptsthere);

    scnt = 0;

    while (1) {
        len = any = 0;
        for (i = 0; sample[i].sample != NULL; ++i) {
            gotem = true;
            cur = sample[i].sample;
            for (j = 0; cur[j] != NULL && gotem; ++j)
                gotem = AllChars(sf, cur[j]);
            if (!gotem && sample[i].sample == _LiBai) {
                cur = _LiBaiShort;
                gotem = true;
                for (j = 0; cur[j] != NULL && gotem; ++j)
                    gotem = AllChars(sf, cur[j]);
            }
            if (gotem) {
                for (s = 0; s < scnt; ++s)
                    if (scriptsdone[s] == sample[i].otf_script) break;
                if (s == scnt) scriptsdone[scnt++] = sample[i].otf_script;

                foundsomething = true;
                ++any;
                for (j = 0; cur[j] != NULL; ++j) {
                    if (ret) utf82u_strcpy(ret + len, cur[j]);
                    len += ff_utf8_strlen(cur[j], -1);
                    if (ret) ret[len] = '\n';
                    ++len;
                }
                if (ret) ret[len] = '\n';
                ++len;
                if (ret && langsyscallback != NULL)
                    (langsyscallback)(tf, len, sample[i].otf_script,
                                      sample[i].lang);
            }
        }

        rcnt = 0;
        for (s = 0; s < therecnt; ++s)
            if (!ScriptInList(scriptsthere[s], scriptsdone, scnt)) {
                if (ret) {
                    if (randoms[rcnt] != NULL) {
                        utf82u_strcpy(ret + len, randoms[rcnt]);
                        len += u_strlen(ret + len);
                        ret[len++] = '\n';
                        ret[len] = '\0';
                        if (langsyscallback != NULL)
                            (langsyscallback)(tf, len, scriptsthere[s],
                                              langs[rcnt]);
                    }
                    free(randoms[rcnt]);
                    randoms[rcnt] = NULL;
                } else {
                    randoms[rcnt] =
                        RandomParaFromScript(scriptsthere[s], &langs[rcnt], sf);
                    for (pt = randoms[rcnt]; *pt == ' '; ++pt);
                    if (*pt == '\0')
                        *randoms[rcnt] = '\0';
                    else {
                        len += ff_utf8_strlen(randoms[rcnt], -1) + 2;
                        foundsomething = true;
                    }
                }
                ++rcnt;
            }

        if (!foundsomething) {
            /* For example, Apostolos's Phaistos Disk font. There is no OT
             * script*/
            /*  code assigned for those unicode points */
            int gid;
            SplineChar* sc;

            pt = buffer;
            for (gid = i = 0; gid < sf->glyphcnt &&
                              pt < buffer + sizeof(buffer) - 4 && i < 50;
                 ++gid) {
                if ((sc = sf->glyphs[gid]) != NULL && sc->unicodeenc != -1) {
                    pt = utf8_idpb(pt, sc->unicodeenc, 0);
                    ++i;
                }
            }
            *pt = '\0';
            if (i > 0) {
                if (ret) {
                    utf82u_strcpy(ret + len, buffer);
                    len += u_strlen(ret + len);
                    ret[len++] = '\n';
                    ret[len] = '\0';
                    if (langsyscallback != NULL)
                        (langsyscallback)(tf, len, DEFAULT_SCRIPT,
                                          DEFAULT_LANG);
                } else
                    len += ff_utf8_strlen(buffer, -1) + 1;
            }
        }

        if (ret) {
            ret[len] = '\0';
            return (ret);
        }
        if (len == 0) {
            /* Er... We didn't find anything?! */
            return ((unichar_t*)calloc(1, sizeof(unichar_t)));
        }
        ret = (unichar_t*)malloc((len + 1) * sizeof(unichar_t));
    }
}
