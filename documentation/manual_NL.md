# Lindi Waterpas - Gebruikershandleiding (Nederlands)

**Versie:** DEV  
**© Syquens B.V. 2025**  
**Auteur:** V.N. Verbon

---

## Inhoudsopgave

1. [Inleiding](#inleiding)
2. [Aan de slag](#aan-de-slag)
3. [Apparaatoverzicht](#apparaatoverzicht)
4. [Hoofdinterface](#hoofdinterface)
   - [Start Tab](#start-tab)
   - [Waterpas Tab](#waterpas-tab)
   - [Info Tab](#info-tab)
5. [Functies](#functies)
   - [Analoge/Digitale Klok](#analogedigitale-klok)
   - [Waterpas](#waterpas)
   - [WiFi-verbinding](#wifi-verbinding)
   - [Instellingen](#instellingen)
6. [Probleemoplossing](#probleemoplossing)
7. [Technische specificaties](#technische-specificaties)

---

## Inleiding

Welkom bij Lindi Waterpas, een slimme digitale waterpas met geïntegreerde klokfunctie. Dit apparaat combineert nauwkeurige nivelleringsmetingen met tijdweergave, waardoor het perfect is voor werkplaatsen, bouwplaatsen en doe-het-zelf projecten.

**Belangrijkste functies:**
- Hoognauwkeurige twee-assige nivellering (Kanteling en Helling)
- WiFi-enabled met automatische tijdsynchronisatie
- Schakelbare analoge/digitale klokweergave
- Aanpasbare weergavethema's (Licht/Donker)
- Meertalige ondersteuning (Engels/Nederlands)
- Permanente opslag van instellingen

---

## Aan de slag

### Eerste inschakeling

1. **Schakel het apparaat in** via de USB-C-poort of externe 5V-voeding
2. Het apparaat start op en toont het **Start**-tabblad met klok
3. Wacht 5-10 seconden tot het systeem is geïnitialiseerd
4. Als WiFi-gegevens zijn geconfigureerd, maakt het apparaat automatisch verbinding

### Eerste configuratie

Navigeer naar het **Info**-tabblad om te configureren:
- **Tijdzone** - Stel uw lokale GMT-offset in
- **Wintertijd** - Schakel zomertijdcompensatie in/uit
- **Taal** - Schakel tussen Engels (EN) en Nederlands (NL)

> **Let op:** Taalwijzigingen werken de meeste labels onmiddellijk bij. Tabbladnamen (Waterpas, Info, etc.) vereisen een herstart - koppel de stroomverbinding los en weer aan.

---

## Apparaatoverzicht

### Display

- **Scherm:** 3.2" TFT LCD (320×240 pixels)
- **Touch:** Resistief touchscreen (XPT2046)
- **Achtergrondverlichting:** Altijd aan LED-achtergrondverlichting

### Sensoren

- **MPU6050:** 6-assige accelerometer/gyroscoop voor nauwkeurige nivellering
- **Meetbereik:** ±30° op beide assen
- **Updatesnelheid:** 10Hz (100ms verversing)

### Connectiviteit

- **WiFi:** 2.4GHz 802.11 b/g/n
- **NTP-synchronisatie:** Automatische tijdsynchronisatie via internet

---

## Hoofdinterface

De interface bestaat uit drie hoofdtabbladen die toegankelijk zijn via aanraking:

### Start Tab

**Doel:** Primaire tijdweergave

**Functies:**
- **Analoge Klok** - Klassieke ronde wijzerplaat met uur-, minuut- en secondewijzers
- **Digitale Klok** - Grote digitale tijdweergave (UU:MM:SS formaat)
- **Schakelknop** - Tik op de klok om te schakelen tussen analoge en digitale modi
- **Automatisch opslaan** - Uw klokvoorkeur wordt automatisch opgeslagen

**Gebruik:**
1. Tik ergens op de klokweergave
2. De klok schakelt tussen analoog en digitaal
3. Uw voorkeur wordt onthouden na herstart

---

### Waterpas Tab

**Doel:** Nauwkeurige waterpasmetingen

**Functies:**
- **Kantelingsindicator** - Verticale balk die voorwaartse/achterwaartse kanteling toont
- **Hellingsindicator** - Horizontale balk die links/rechts helling toont
- **Numerieke Aflezing** - Real-time graadenmetingen (±0,1°)
- **Visuele Feedback** - Balkpositie geeft richting en grootte aan

**Hoe te lezen:**

**Kanteling (Voorwaarts/Achterwaarts):**
- Balk gecentreerd = Waterpas (0°)
- Balk omhoog = Achterwaarts gekanteld (positieve graden)
- Balk omlaag = Voorwaarts gekanteld (negatieve graden)

**Helling (Links/Rechts):**
- Balk gecentreerd = Waterpas (0°)
- Balk rechts = Naar rechts gekanteld (positieve graden)
- Balk links = Naar links gekanteld (negatieve graden)

**Gebruikstips:**
1. Plaats apparaat op het te meten oppervlak
2. Wacht 1-2 seconden tot de metingen stabiliseren
3. Pas oppervlak aan tot beide balken op 0° centreren
4. Metingen worden 10 keer per seconde bijgewerkt voor real-time feedback

**Meetbereik:**
- Maximum: ±30° op elke as
- Precisie: ±0,1°
- Aanbevolen werkbereik: ±10° voor beste nauwkeurigheid

---

### Info Tab

**Doel:** Apparaatinstellingen en systeeminformatie

**Secties:**

#### Versie-informatie
- Firmwareversie met bouwdatum/-tijd
- Copyright en auteursinformatie

#### Netwerkstatus
- WiFi-verbindingsstatus (Verbonden/Niet verbonden)
- IP-adres wanneer verbonden
- Maakt automatisch opnieuw verbinding bij verbroken verbinding

#### Instellingen

**Tijdzone:**
- Vervolgkeuzemenu met GMT-12 tot GMT+12
- Standaard: GMT+1 (Amsterdam)
- Beïnvloedt klokweergave onmiddellijk

**Wintertijd:**
- Schakelaar voor zomertijd
- AAN = Trek 1 uur af van tijdzone
- UIT = Gebruik standaardtijd
- Wordt automatisch opgeslagen

**Toon FPS/CPU:**
- Prestatiemonitor schakelaar
- Toont frames per seconde en CPU-gebruik
- Standaard: UIT (voor schonere weergave)

**Donker Thema:**
- Schakel tussen lichte en donkere weergavethema's
- Donker thema vermindert oogvermoeidheid bij weinig licht
- Wordt automatisch opgeslagen
- **Let op:** Themawijzigingen vereisen mogelijk een herstart voor volledig effect

**Niveau omkeren:**
- Keert kanteling- en hellingsmetingen om
- Nuttig als sensor achterstevoren is gemonteerd
- Gebruik als linkse kanteling als rechtse kanteling wordt getoond
- Wordt automatisch opgeslagen

**Taal:**
- Schakel tussen Engels (EN) en Nederlands (NL)
- UIT = Engels
- AAN = Nederlands
- **Belangrijk:** Herstart vereist voor taalwijziging

---

## Functies

### Analoge/Digitale Klok

**Automatische Tijdsynchronisatie:**
- Maakt verbinding met NTP-servers (pool.ntp.org, time.google.com)
- Synchroniseert tijd binnen seconden na WiFi-verbinding
- Automatische herverbinding bij mislukte synchronisatie

**Klokmodi:**
- **Analoog:** Traditionele wijzerplaat, vloeiende secondewijzer
- **Digitaal:** Grote cijfers, UU:MM:SS formaat, 24-uurs weergave

**Tijdzone-afhandeling:**
- Basistijdzone ingesteld in Info-tabblad
- Wintertijd-optie trekt 1 uur af
- Voorbeeld: GMT+1 met Wintertijd AAN = GMT+0

---

### Waterpas

**Technologie:**
- MPU6050 6-assige MEMS-sensor
- Accelerometer-gebaseerde kantelingsmeting
- Temperatuur-gecompenseerde metingen

**Kalibratie:**
- Geen gebruikerskalibratie vereist
- Fabrieksgekalibreerd voor ±1° nauwkeurigheid
- Zelfstabiliserend algoritme

**Beste Praktijken:**
- Plaats op stabiel, vlak referentieoppervlak
- Vermijd trillingen tijdens meting
- Sta 2-3 seconden toe voor initiële stabilisatie
- Houd sensor weg van magnetische velden

---

### WiFi-verbinding

**Configuratie:**
- WiFi-gegevens opgeslagen in apparaatfirmware
- Automatische verbinding bij opstarten
- Tot 5 nieuwe pogingen
- Blijft functioneren als WiFi niet beschikbaar is

**Netwerkvereisten:**
- 2.4GHz WiFi-netwerk
- WPA2 Personal-codering ondersteund
- DHCP voor automatische IP-toewijzing
- Internettoegang voor NTP-synchronisatie (optioneel)

**Verbindingsindicatoren:**
- Info-tabblad toont "Verbonden" of "Niet verbonden"
- IP-adres weergegeven wanneer verbonden
- Geen visuele indicator op andere tabbladen

---

### Instellingen

**Permanente Opslag:**
- Alle instellingen opgeslagen in niet-vluchtig geheugen (NVS)
- Instellingen overleven stroomuitval en herstarts
- Geen "Opslaan"-knop vereist (automatisch opslaan)

**Opgeslagen Instellingen:**
- Klokmodus (analoog/digitaal)
- Tijdzone-offset
- Wintertijd ingeschakeld/uitgeschakeld
- Donker thema ingeschakeld/uitgeschakeld
- Sensorinversie ingeschakeld/uitgeschakeld
- Taalvoorkeur (EN/NL)

**Taal Wisselen:**
- Schakel de **EN/NL** schakelaar in het Info-tabblad
- Instellingslabels worden onmiddellijk bijgewerkt (Tijdzone, Wintertijd, etc.)
- Tabbladnamen worden bijgewerkt na herstart (stroomverbinding loskoppelen en weer aansluiten)
- Uw taalvoorkeur wordt automatisch opgeslagen

**Fabrieksinstellingen:**
- Niet beschikbaar via gebruikersinterface
- Neem contact op met ondersteuning voor resetprocedures

---

## Probleemoplossing

### Klok wordt niet bijgewerkt
- **Controleer WiFi-verbinding** in Info-tabblad
- **Verifieer internettoegang** op uw netwerk
- **Wacht 60 seconden** voor initiële NTP-synchronisatie
- **Herstart apparaat** als tijd onjuist blijft

### Waterpasmetingen onjuist
- **Controleer "Niveau omkeren"-instelling** - schakel om indien omgekeerd
- **Plaats op bekend waterpas oppervlak** om te verifiëren
- **Herstart apparaat** om sensor te resetten
- **Vermijd magnetische interferentie** (luidsprekers, motoren)

### WiFi wil niet verbinden
- **Verifieer netwerkbeschikbaarheid** (alleen 2.4GHz)
- **Controleer WiFi-gegevens** (vereist firmware-update)
- **Ga dichter bij router** voor beter signaal
- **Wacht op alle 5 nieuwe pogingen** (30-60 seconden)

### Touch reageert niet
- **Tik stevig** op touchscreen
- **Reinig scherm** met zachte, droge doek
- **Herstart apparaat** bij aanhoudende niet-responsiviteit
- **Vermijd schermbeschermers** die gevoeligheid kunnen verminderen

### Display te helder/donker
- **Gebruik Donker Thema** voor lagere helderheidsuiterlijk
- **Geen handmatige helderheidsregeling** in huidige versie
- Helderheid is hardware-gecontroleerd

### Taal verandert niet
- **Herstart vereist** na taalschakelaar
- Schakel de Taal-schakelaar in Info-tabblad
- Schakel apparaat uit en weer aan
- Controleer dat schakelaar naar gewenste positie is verplaatst

---

## Technische specificaties

### Hardware

**Microcontroller:**
- ESP32-D0WD-V3 (Dual-core Xtensa LX6)
- Klok: 240 MHz
- Flash: 4MB
- RAM: 520KB SRAM

**Display:**
- Controller: ILI9341
- Grootte: 3.2" diagonaal
- Resolutie: 320×240 pixels
- Kleuren: 262K (18-bit)
- Interface: SPI

**Touch:**
- Controller: XPT2046
- Type: 4-draads resistief
- Interface: SPI
- Drukgevoelig

**Sensor:**
- Model: MPU6050
- Interface: I2C (400kHz)
- Bereik: ±2g (accelerometer)
- Resolutie: 16-bit
- Updatesnelheid: 100Hz (10ms intern)

**Connectiviteit:**
- WiFi: IEEE 802.11 b/g/n
- Frequentie: 2.4GHz
- Beveiliging: WPA/WPA2
- Antenne: PCB trace-antenne

### Software

**Framework:**
- ESP-IDF v5.5
- FreeRTOS
- LVGL v7 (GUI-bibliotheek)

**Functies:**
- NVS flash-opslag
- SNTP-tijdsynchronisatie
- I2C-apparaatstuurprogramma
- Multi-task architectuur

**Prestaties:**
- Opstarttijd: 3-5 seconden
- Displayverversing: 20-25 FPS
- Waterpas-updatesnelheid: 10 Hz
- Klok-update: 1 Hz

### Voeding

**Invoer:**
- Spanning: 5V DC
- Stroom: 200-400mA (typisch)
- Connector: USB-C of GPIO-header

**Verbruik:**
- Inactief: ~200mA
- Actief (alle functies): ~400mA
- Diepe slaap: Niet geïmplementeerd

---

## Ondersteuning

Voor technische ondersteuning, firmware-updates of functieverzoeken:

**Contact:**
- Bedrijf: Syquens B.V.
- Auteur: V.N. Verbon
- Jaar: 2025

**Documentatie:**
- Volledige technische documentatie beschikbaar in /documentation map
- Broncode: ESP-IDF gebaseerd, aangepaste firmware

---

**Einde Handleiding**
