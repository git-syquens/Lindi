# Lindi Waterpas - Snelle Referentiegids

**© Syquens B.V. 2025**

---

## 📱 Interface Overzicht

Het apparaat heeft **3 hoofdtabbladen** waartussen u kunt schakelen door te tikken:

| Tabblad | Doel | Belangrijkste Functies |
|---------|------|------------------------|
| **Start** | Klokweergave | Analoge/Digitale klok, tik om modi te wisselen |
| **Waterpas** | Waterpas | Kanteling (voor/achter) en Helling (links/rechts) metingen |
| **Info** | Instellingen & status | WiFi-status, tijdzone, thema, taal, versie-info |

---

## 🕐 De Klok Gebruiken (Start Tabblad)

**Klokmodus Wisselen:**
- Tik ergens op de klokweergave
- Schakelt tussen **Analoog** (ronde wijzerplaat) en **Digitaal** (UU:MM:SS)
- Uw voorkeur wordt automatisch opgeslagen

**Tijdsynchronisatie:**
- Automatisch via WiFi en NTP-servers
- Werkt bij binnen seconden na WiFi-verbinding
- Controleer WiFi-status in Info-tabblad als tijd onjuist is

---

## 📏 De Waterpas Gebruiken (Waterpas Tabblad)

**De Weergave Lezen:**

```
      Kanteling (Verticale Balk)
             ▲
      Achterwaarts
             |
    -------- 0° --------  ← Waterpas
             |
        Voorwaarts
             ▼

    Helling (Horizontale Balk)
    Links ← --0°-- → Rechts
```

**Meettips:**
1. Plaats apparaat plat op oppervlak
2. Wacht 1-2 seconden voor stabilisatie
3. Beide balken gecentreerd = Oppervlak is waterpas
4. Getallen tonen exacte kanteling in graden (±0,1°)

**Meetbereik:** ±30° (aanbevolen ±10° voor beste nauwkeurigheid)

### Kalibratie (Knoppen Linkerzijde)

**Wanneer Kalibreren:**
- Sensor kan niet perfect waterpas gemonteerd worden
- Compensatie nodig voor vaste montagehoek

**Hoe Kalibreren:**
1. Zet camper handmatig waterpas (met steunen/blokken + externe waterpas)
2. Wacht tot metingen stabiel zijn
3. Druk op **Kalibreer** knop (linkerzijde)
4. Bevestig wanneer gevraagd
5. Huidige positie is nu nulreferentie

**Reset Kalibratie:**
- Druk op **Reset** knop (onder Kalibreer)
- Bevestig (vorige offset gaat verloren)
- Keert terug naar ruwe sensormetingen

⚠️ **Belangrijk:** Zet camper altijd handmatig waterpas VOOR kalibratie!

---

## ⚙️ Instellingen (Info Tabblad)

### Essentiële Instellingen

| Instelling | Opties | Beschrijving |
|------------|--------|--------------|
| **Tijdzone** | GMT-12 tot GMT+12 | Uw lokale tijd-offset (standaard: GMT+1) |
| **Wintertijd** | AAN/UIT | Inschakelen voor zomertijd |
| **Taal** | EN/NL | Engels of Nederlands (labels direct, tabbladnamen na herstart) |
| **Niveau omkeren** | AAN/UIT | Kantelingsrichting omkeren als sensor achterstevoren gemonteerd |
| **Donker Thema** | AAN/UIT | Donker display voor weinig licht omgevingen |

### Snelle Instellingengids

**Om tijdzone te wijzigen:**
1. Ga naar Info-tabblad
2. Tik op de Tijdzone vervolgkeuzelijst
3. Selecteer uw GMT-offset
4. Tijd wordt onmiddellijk bijgewerkt

**Om taal te wijzigen:**
1. Ga naar Info-tabblad
2. Schakel de EN/NL schakelaar
3. Instellingslabels worden onmiddellijk bijgewerkt
4. Voor tabbladnamen, herstart apparaat (stroomverbinding loskoppelen/aansluiten)

**Om taal te wijzigen:**
1. Ga naar Info-tabblad
2. Schakel Taal-schakelaar (UIT=Engels, AAN=Nederlands)
3. **Herstart het apparaat** om wijzigingen toe te passen

**Als waterpasmetingen omgekeerd zijn:**
1. Ga naar Info-tabblad
2. Schakel "Niveau omkeren"-schakelaar AAN
3. Wijzigingen zijn onmiddellijk van kracht

---

## 🔧 Probleemoplossing

| Probleem | Oplossing |
|----------|-----------|
| ⏰ **Klok wordt niet bijgewerkt** | Controleer WiFi in Info-tabblad, wacht 60 seconden, herstart indien nodig |
| 📏 **Waterpasmetingen verkeerd** | Schakel "Niveau omkeren"-instelling in Info-tabblad |
| 📡 **WiFi wil niet verbinden** | Controleer 2.4GHz netwerk, wacht op 5 nieuwe pogingen (~60 sec) |
| 🖐️ **Touch reageert niet** | Tik stevig, reinig scherm, herstart apparaat |
| 🌐 **Taal verandert niet** | **Moet apparaat herstarten** na taalschakelaar omzetten |

---

## 🎯 Snelle Startstappen

1. **Schakel in** het apparaat (USB-C of 5V voeding)
2. **Wacht 5-10 seconden** voor opstarten en WiFi-verbinding
3. **Stel tijdzone in** in Info-tabblad (standaard GMT+1)
4. **Schakel naar Waterpas-tabblad** om waterpas te gebruiken
5. **Tik op klok** in Start-tabblad om weergavemodus te wijzigen

---

## 📊 Technische Snelle Feiten

- **Display:** 3.2" touchscreen (320×240)
- **Sensor:** MPU6050 6-assig (±30° bereik, ±0,1° precisie)
- **WiFi:** Alleen 2.4GHz, WPA2 ondersteund
- **Updatesnelheid:** Waterpas 10Hz, Klok 1Hz
- **Voeding:** 5V USB-C, 200-400mA
- **Opslag:** Alle instellingen automatisch opgeslagen in flash-geheugen

---

## 💡 Pro Tips

✓ **Klokmodus wordt opgeslagen** - Uw analoge/digitale voorkeur blijft behouden  
✓ **Alle instellingen auto-opslaan** - Geen opslagknop nodig  
✓ **Waterpas werkt 10x/sec bij** - Real-time feedback voor nauwkeurige aanpassingen  
✓ **Donker thema vermindert schittering** - Goed voor werkomgevingen met weinig licht  
✓ **Niveau omkeren voor montage** - Draai sensormetingen om als apparaat achterstevoren is  
✓ **Taal vereist herstart** - Enige instelling die stroomcyclus vereist  

---

## 📞 Ondersteuning

**Syquens B.V. 2025**  
Auteur: V.N. Verbon

Voor gedetailleerde informatie, zie volledige gebruikershandleiding:
- `documentation/manual_EN.md` (English)
- `documentation/manual_NL.md` (Nederlands)

---

**Snelle Referentie v1.0**
