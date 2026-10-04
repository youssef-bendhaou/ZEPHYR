# ESP32 + MPU6050 + Nokia 5110 avec Zephyr RTOS

Petite application embarquée sous **Zephyr RTOS** : un ESP32 lit la température et l'accélération d'un capteur **MPU6050** (I2C) et les affiche sur un écran **Nokia 5110** (SPI), avec un rafraîchissement toutes les 500 ms.

```
ESP32 MPU6050

T: 27.4 C
X: +0.02 g
Y: -0.01 g
Z: +1.00 g
```

## Matériel

| Composant | Rôle | Bus |
|---|---|---|
| ESP32 DevKit V1 (ESP32-WROOM-32) | Microcontrôleur | — |
| MPU6050 | Accéléromètre + température | I2C |
| Nokia 5110 (contrôleur PCD8544) | Écran 84 × 48 pixels | SPI |

## Câblage

### Nokia 5110

| Nokia 5110 | ESP32 |
|---|---|
| RST | GPIO16 (RX2) |
| CE | GPIO5 |
| DC | GPIO4 |
| DIN | GPIO23 |
| CLK | GPIO18 |
| VCC | 3V3 |
| BL | 3V3 (ou GND selon le module) |
| GND | GND |

### MPU6050

| MPU6050 | ESP32 |
|---|---|
| VCC | 3V3 |
| GND | GND |
| SDA | GPIO21 |
| SCL | GPIO22 |

> ⚠️ Tous les modules sont alimentés en **3,3 V** (jamais en 5 V) et tous les GND sont reliés ensemble.

## Structure du projet

```
esp32_mpu_nokia/
├── CMakeLists.txt                      # Fichiers à compiler
├── prj.conf                            # Fonctionnalités Zephyr activées (I2C, SPI, capteur, logs)
├── app.overlay                         # Devicetree : description du câblage
├── dts/bindings/nokia,pcd8544-lcd.yaml # Déclaration du Nokia 5110 pour Zephyr
└── src/
    ├── main.c                          # Boucle principale : lecture capteur + affichage
    ├── pcd8544.c                       # Driver de l'écran Nokia 5110
    └── pcd8544.h                       # Interface du driver
```

Zephyr ne fournit pas encore de driver officiel pour le PCD8544 : un driver minimal (framebuffer + police 5×7) est donc inclus dans le projet.

## Compilation

Dans un environnement Zephyr (par exemple un conteneur Docker avec le SDK Zephyr et `west`) :

```bash
# Une seule fois : récupérer les blobs Espressif
west blobs fetch hal_espressif

# Compiler
west build -p always -b esp32_devkitc_wroom/esp32/procpu esp32_mpu_nokia
```

Selon la version de Zephyr, le nom de la carte peut être `esp32_devkitc_wroom` (anciennes versions) ou `esp32_devkitc/esp32/procpu` (versions récentes). Liste des cartes disponibles : `west boards | grep esp32`.

## Flash

### Sous Linux

```bash
west flash --esp-device /dev/ttyUSB0
```

### Sous Windows (compilation dans Docker)

Docker Desktop n'a pas accès aux ports USB : on flashe depuis Windows avec `esptool`.

```bash
pip install esptool
esptool --chip esp32 --port COM5 --baud 460800 write_flash 0x1000 build/zephyr/zephyr.bin
```

Si le flash reste bloqué sur `Connecting...`, maintenir le bouton **BOOT** de la carte.

## Logs série

```bash
python -m serial.tools.miniterm COM5 115200
```

## Dépannage

| Problème | Solution |
|---|---|
| `MPU6050 non prêt` | Vérifier SDA/SCL et l'adresse I2C (0x68, ou 0x69 si AD0 est à 3V3) |
| Écran vide ou tout noir | Ajuster `LCD_CONTRAST` dans `main.c` (entre 0x30 et 0x50) |
| Pas de rétroéclairage | Brancher BL sur GND au lieu de 3V3 |
| Erreur sur `SPI_DT_SPEC_GET` | Retirer le dernier argument `, 0` de la macro dans `pcd8544.c` |

## Remarque

La température affichée est celle de la puce du MPU6050 : elle est légèrement supérieure à la température ambiante.
