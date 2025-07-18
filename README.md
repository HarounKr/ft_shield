# ft_shield

Ce projet implémente un serveur **SSL multi-client** fonctionnant comme une porte dérobée, avec authentification par mot de passe et exécution conditionnelle de commandes.
Il intègre également un module de keylogging conçu pour enregistrer de manière furtive les frappes clavier effectuées sur la machine cible.

## Fonctionnalités

- Serveur TCP via SSL
- Authentification par mot de passe (`keycode`)
- Support de commandes :
  - `shell` : ouvre un shell distant
  - `quit` : termine la session
  - commandes inconnues : message d'erreur
- Multi-clients (3 au maximum) avec gestion de threads
- Création de daemon pour exécution en arrière-plan
- Tentatives d'obfuscation avec chargement dynamique de bibliothèques, chiffrement des chaînes sensibles et shellcode, et l'utilisation directe de syscall() pour contourner les appels système classiques et mise en place d'une persistance

## Sécurité

- Toutes les communications client/serveur passent par SSL.
- Le client doit entrer un mot de passe (`keycode`) pour obtenir un accès.
- Si le mot de passe est incorrect, la connexion est refusée.

## Prérequis

Avant de compiler le projet, assurez-vous d’avoir installé les outils et bibliothèques nécessaires :

```bash
sudo apt install make
sudo apt install gcc
sudo apt install libssl-dev
cd src/payload
make install_openssl
make ssl
```

## Compilation et Execution

```bash
cd src/payload
make
sudo ./ft_shield
```

## Client

Vous pouvez tester la connexion avec openssl s_client :

```bash
openssl s_client -connect <IP>:<PORT>
```

## Comportement du Daemon

**Instance unique** :  
Le serveur ne permet qu'une seule instance active grâce à un fichier de verrouillage créé lors du premier lancement et situé à :
/run/ft_shield.lock

**Ignorance des signaux** :  
Le daemon ignore tous les signaux standards. Pour l'arrêter manuellement :

```bash
cd src/payload
make stop
make fclean
```
