# 3D printer housing

This project was carried out using a NodeMCU with Wi-Fi connectivity, and can therefore be adapted to other microcontrollers. The function of this circuit is to regulate the temperature of a 3D printer's enclosure using two fans: one to expel internal air and the other to introduce fresh air.

All the functions can be accessed via a web page hosted on the NodeMCU, enabling you to :

- View the temperature and humidity of the enclosure
- Change the maximum temperature of the enclosure
- View an IP camera if an installation is in place

## 3D model

This 3D model was created using Fusion360. M2.5x5.7x4.6 threaded inserts with M2.5x8 screws were used to attach the top to the bottom.
3D Fusion 360 model: https://ibenji.fr/BoitierFusion360.f3d

## Code

The program for this circuit makes a web page, you just need to fill in the SSID and PASSWORD for the wifi. The camera stream can be removed from the code.


# Français :

## Caisson Imprimante 3D

Ce projet a été réalisé avec un NodeMCU doté d'une connectivité Wi-Fi, et peut donc être adapté à d'autres microcontrôleurs. Ce circuit a pour fonction de réguler la température du caisson d'une imprimante 3D à l'aide de deux ventilateurs : l'un expulsant l'air intérieur et l'autre introduisant de l'air frais.

Toutes les fonctionnalités sont accessibles via une page web hébergée sur le NodeMCU, permettant de :

- Voir la température et l'humidité de l'enceinte
- Changer la température maximale de l'enceinte
- Afficher la vue d'une caméra IP si une installation est en place

## Modèle 3D

Ce modèle 3D a été réalisé sous Fusion360. Pour fixer la partie haute à la partie basse, des inserts filetés M2.5x5.7x4.6 avec des vis M2.5x8 ont été utilisés.
Model 3D Fusion 360 : https://ibenji.fr/BoitierFusion360.f3d

## Code

Le programme de ce circuit fais donc une page web, il faut renseigner le SSID et le PASSWORD pour le wifi et pour la sécurisation des pages web. Possibilité de changer le programme en allant sur http://YOUR_ESP_IP:PORT/update. Le stream de la caméra peut être enlever dans le code.
