Měření a vyhodnocení \label{measureeval00}
====================

Vymezení pojmů, popis prostředí
-------------------------------

### OMNeT++

OMNeT++ je diskrétní simulátor napsaný v\ jazyce C++ a ...

Vyhodnocení
-----------

ADEV, MDEV, TDEV, minTDEV, bandTDEV + percentileTDEV

Demonstrační úlohy
==================

V\ závislosti na vhodnosti metody pro synchronizaci času ve SCADA QNX byly připraveny konfigurace a demonstrační aplikace pro PTP, NTP a GPS.

Každá aplikace se zaměřuje na jinou třídu SCADA systémů. Vychází se přitom z\ provedené klasifikace.

Aplikace 1
----------

soft systémy => postačuje NTP

Aplikace 2
----------

firm systémy => je využito PTP, avšak lze využít za okolností uvedených v\ ?????????????????????vysledky_mereni_a_zhodnoceni_NTP?????????????? i NTP

Aplikace 3
----------

hard systémy => je využito GPS synchronizace, avšak lze využít za okolností uvedených v\ ???????????????????????vysledky_mereni_a_zhodnoceni_PTP???????? i PTP

Model v\ OMNeT++
================

Na základě provedených měření byl vytvořen model scada systému parametrizovatelný třídou, náročností na šířku přenosového pásma (minimální, průměrná a maximální) a ???????

Všechna provedená měřění (viz. kap. \ref{measureeval00}) byla odsimulována a výsledky porovnány v\ tabulce níže. Je zřejmé, že model odpovídá reálným měřením a lze ho tedy použít pro návrh komplexních síťových topologií pro SCADA systémy.

Network #       load (%)     TA       TB        PTD simulation  PTD real
--------------- ------------ -------- --------- --------------- -------------
a               20           x1       x2        ???             ???
b               50           y1       y2        ???             ???
c               70           z1       z2        ???             ???
...             ...          ...      ...       ...             ...

:Výsledky simulace a reálných měření

(@l00) Elektro Drapač. *Elektro Drapač s.r.o* [online]. 2012------, [cit. 2012-12-08]. Dostupné na: <http://www.eldr.cz/>.
(@l01) Autor0, Autor1. *Nazev dila*. [cit. 2013-01-16]. str. 50-60
(@) Kolektiv autorů: *Pravidla českého pravopisu*. Academia, 2005, ISBN 80-200-1327-X.
(@) Rybička, J.: *LaTeX pro začátečníky*. Konvoj, 1999, ISBN 80-85615-77-0.
(@) Rábova, Z.; Hanáček, P.; Peringer, P.; aj.: Užitečné rady pro psaní odborného textu [online]. http://www.fit.vutbr.cz/info/statnice/psani_textu.html, 2008-11-01 [cit. 2008-11-28].
