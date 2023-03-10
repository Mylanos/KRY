# Projekt KRY

## Spracovanie argumentov

Na spracovanie argumentov som vyuzil lightweight kniznicu cxxopts (credits - <https://github.com/jarro2783/cxxopts>). Tato kniznica sa vyznacuje tym, ze sa sklada len z jedneho header subora, ktory poskytuje radu metod a tried pre jednoduche spracovanie argumentov. V projekte som vyuzil triedu `cxxopts::Options`, v ktorej metodami `add_options`, `parse_positional` a `parse` spracujem specifikovane argumenty zo zadania. Na jednotlive hodnoty argumentov som si vytvoril triedu `Specification`, ktora tieto hodnoty uklada pre pouzitie dalej v kode. Program obsahuje zopar jednoduchych kontrol na spravnost argumentov, ale zcela to nie je blbuvzdorne. Pre vypis moznych argumentov pouzite prepinac `./kry -h | --help`.

## Spracovanie vstupu

Na zaklade predanych argumentov sa spracovava vstup zo subora, z argumentu ktory nie je niektory z podporovanych prepinacov uvedeny v uvodzovkach alebo program cita vstup z stdin.

## Mod sifrovania

Mod sifrovania sa spusta prepinacom `-e`. O tuto funkcionalitu sa stara funkcia `encryption()`. Tato funkcia pre kazdy charakter v retazci vola funkciu `encrypt_character()`, ktora na zaklade vzorca `encr_char = (a_key \* input_char + b_key) mod 26` zakoduje dany charakter:

## Mod desifrovania

Mod desifrovania sa spusta prepinacom `-d`. O tuto funkcionalitu sa stara funkcia `decryption()`. Tato funkcia pre kazdy charakter v retazci vola funkciu `decrypt_character()`, ktora na zaklade vzorca `decr_char = a_key−1(input_char – b_key) mod 26` dekoduje dany charakter.

## Mod desifrovania bez znalosti klucov

Mod desifrovania bez znalosti klucov sa spusta prepinacom `-c`. O tuto funkcionalitu sa stara kombinacia predchadzajucej funkcie `decryption()` a funkcie `frequence_analysis`. Funkcia `frequence_analysis` pocita vyskyt jednolivych znakov a bigramov v zasifrovanom texte. Ako prve sa vyhodnocuje pocet znakov a nasledne pocet bigramov. Bigramy sa v texte urcuju stylom posunu o 2 znaky, a pri narazeni na bigram s medzerou sa bigram ignoruje(priklad slovo 'kachnicka' sa rozdeli na 'ka', 'ch', 'ni', 'čk'). Na konci sa tieto frekvencie bigramov a znakov predaju do funkcie `determine_keys()`, kde sa vyberie 4 najcastejsie sa vyskytujucich znakov v sifrovanom texte, ktore budeme predpokladat ze zodpovedaju najcastejsie sa vyskytujucim znakom v ceskom jazyku. Volam 6x funkciu `reverse_calc()` vzdy pre 2 rozne pary najcastejsich znakov (znak zo sifry , predpokladany prislusny znak v cestine) ktora skusi spocitat kluce na zaklade vzorcov
`a_key = (encr_char1 - encr_char2) * (input_char1 - input_char2)^-1 mod 26`
`b_key = encr_char1 - a_key*input_char1 mod 26`
odvodenych z `decr_char = a_key−1(input_char – b_key) mod 26`.
Vysledkom tychto funkci, su dva vectory obsahujuce potencionalne kluce 'a' a rozsah potencionalnych klucov 'b'.
Nasledne pre kazdy par klucov desifrujem najcastejsie bigramy z sifrovaneho textu, ktore sa porovnavaju s 15 najcastejsimi bigramami v ceskom jazyku.
Par klucov ktory ma najvacsi pocet zhod je zvoleny ako riesenie a tento par sa pozuije na desifrovanie textu, znova pomocou funckie `decrypt_character(x, a, b)` z predoslej casti.
