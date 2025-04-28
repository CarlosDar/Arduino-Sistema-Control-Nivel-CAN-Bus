# Arduino-Control-Nivel-Deposito-Master-Slave

## Descripción Breve
Sistema de control de nivel implementado con dos Arduino Mega 2560 en arquitectura master-slave. El sistema utiliza comunicación CAN bus para coordinar el control de nivel de un depósito mediante válvulas de entrada/salida, con interfaz de operador y monitorización en tiempo real.

## Esquema del Sistema
```
┌─────────────────────────────────────────────────────────────────────────────┐
│                            NODO MASTER (A)                                  │
│                                                                             │
│  ┌─────────────┐     ┌─────────────┐           ┌─────────────────┐         │
│  │   Arduino   │     │    LCD      │           │    Terminal     │         │
│  │   Mega 2560 │◄────┤  Display    │◄──────────┤      UART       │         │
│  │             │     │             │           │                 │         │
│  │  Control    │     │  Visualización│         │  Configuración  │         │
│  │  Principal  │     │  Estado      │         │  y Monitoreo    │         │
│  └──────┬──────┘     └─────────────┘           └─────────────────┘         │
│         │                                                                  │
│         │ CAN Bus (500 kbps)                                               │
│         ▼                                                                  │
└─────────┼──────────────────────────────────────────────────────────────────┘
          │
          │
┌─────────┼──────────────────────────────────────────────────────────────────┐
│         ▼                                                                  │
│  ┌─────────────┐     ┌─────────────┐           ┌─────────────────┐         │
│  │   Arduino   │     │  Sensores   │           │    Válvulas     │         │
│  │   Mega 2560 │◄────┤   Nivel     │◄──────────┤  VE y VS        │         │
│  │             │     │             │           │                 │         │
│  │  Control    │     │  Medición   │           │  Control de     │         │
│  │  Local      │     │  Continua   │           │  Flujo          │         │
│  └─────────────┘     └─────────────┘           └─────────────────┘         │
│                                                                             │
│                    NODO SLAVE (B)                                           │
└─────────────────────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────────────────────┐
│                            Flujo de Datos                                   │
│                                                                             │
│  Master → Slave:                                                            │
│  - Comandos de control (ID_Actua_VE, ID_Actua_VS)                          │
│  - Ajustes de periodo (ID_CambioPeriodo)                                    │
│                                                                             │
│  Slave → Master:                                                            │
│  - Estado de válvulas (ID_TramaCAN_VE, ID_TramaCAN_VS)                     │
│  - Nivel actual (ID_TramaCAN_Nivel)                                        │
│                                                                             │
│  Polling:                                                                   │
│  - Solicitud de estado (ID_AB_Polling)                                      │
└─────────────────────────────────────────────────────────────────────────────┘
```

## System Diagram
```
┌─────────────────────────────────────────────────────────────────────────────┐
│                            MASTER NODE (A)                                  │
│                                                                             │
│  ┌─────────────┐     ┌─────────────┐           ┌─────────────────┐         │
│  │   Arduino   │     │    LCD      │           │    Terminal     │         │
│  │   Mega 2560 │◄────┤  Display    │◄──────────┤      UART       │         │
│  │             │     │             │           │                 │         │
│  │  Main       │     │  Status     │           │  Configuration  │         │
│  │  Control    │     │  Display    │           │  and Monitoring │         │
│  └──────┬──────┘     └─────────────┘           └─────────────────┘         │
│         │                                                                  │
│         │ CAN Bus (500 kbps)                                               │
│         ▼                                                                  │
└─────────┼──────────────────────────────────────────────────────────────────┘
          │
          │
┌─────────┼──────────────────────────────────────────────────────────────────┐
│         ▼                                                                  │
│  ┌─────────────┐     ┌─────────────┐           ┌─────────────────┐         │
│  │   Arduino   │     │   Level     │           │    Valves       │         │
│  │   Mega 2560 │◄────┤  Sensors    │◄──────────┤  VE and VS      │         │
│  │             │     │             │           │                 │         │
│  │  Local      │     │  Continuous │           │  Flow           │         │
│  │  Control    │     │  Measurement│           │  Control        │         │
│  └─────────────┘     └─────────────┘           └─────────────────┘         │
│                                                                             │
│                    SLAVE NODE (B)                                           │
└─────────────────────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────────────────────┐
│                            Data Flow                                        │
│                                                                             │
│  Master → Slave:                                                            │
│  - Control commands (ID_Actua_VE, ID_Actua_VS)                             │
│  - Period adjustments (ID_CambioPeriodo)                                    │
│                                                                             │
│  Slave → Master:                                                            │
│  - Valve status (ID_TramaCAN_VE, ID_TramaCAN_VS)                           │
│  - Current level (ID_TramaCAN_Nivel)                                       │
│                                                                             │
│  Polling:                                                                   │
│  - Status request (ID_AB_Polling)                                           │
└─────────────────────────────────────────────────────────────────────────────┘
```

## Índice
1. [Descripción General](#descripción-general)
2. [Arquitectura Master-Slave](#arquitectura-master-slave)
   - [Nodo Master (Nodo A)](#nodo-master-nodo-a)
   - [Nodo Slave (Nodo B)](#nodo-slave-nodo-b)
3. [Comunicación CAN Bus](#comunicación-can-bus)
   - [Protocolo de Comunicación](#protocolo-de-comunicación)
4. [Control de Nivel del Depósito](#control-de-nivel-del-depósito)
   - [Variables de Control](#variables-de-control)
   - [Lógica de Control](#lógica-de-control)
   - [Estados del Sistema](#estados-del-sistema)
5. [Interfaz de Operador](#interfaz-de-operador)
   - [LCD Display](#lcd-display)
   - [Terminal UART](#terminal-uart)
   - [Comandos del Operador](#comandos-del-operador)
6. [Características Técnicas](#características-técnicas)
   - [Hardware](#hardware)
   - [Software](#software)

## Descripción General
Este proyecto implementa un sistema de control de nivel de un depósito utilizando una arquitectura master-slave con dos nodos Arduino comunicados mediante CAN bus.

## Arquitectura Master-Slave

### Nodo Master (Nodo A)
- **Responsabilidades:**
  - Control principal del sistema
  - Interfaz con el operador (LCD y terminal UART)
  - Gestión de consignas de nivel
  - Control de válvulas de entrada (VE) y salida (VS)
  - Monitoreo de temperatura
  - Gestión de alarmas

### Nodo Slave (Nodo B)
- **Responsabilidades:**
  - Control local de válvulas
  - Medición de nivel del depósito
  - Ejecución de órdenes del master
  - Comunicación de estados al master

## Comunicación CAN Bus

### Protocolo de Comunicación
- **Velocidad:** 500 kbps
- **Modo:** Normal
- **Identificadores CAN:**
  - `ID_AB_Polling`: Solicitud de estado del slave
  - `ID_Actua_VE`: Control de válvula de entrada
  - `ID_Actua_VS`: Control de válvula de salida
  - `ID_CambioPeriodo`: Ajuste de periodo de muestreo
  - `ID_TramaCAN_VE`: Estado de válvula de entrada
  - `ID_TramaCAN_VS`: Estado de válvula de salida
  - `ID_TramaCAN_Nivel`: Nivel actual del depósito

## Control de Nivel del Depósito

### Variables de Control
- **Nivel:** Medido en milímetros (mm)
- **Consigna:** Nivel objetivo (500mm por defecto)
- **Banda de Histeresis:** ±100mm alrededor de la consigna
- **Rango de Consigna:** 500mm a 1250mm

### Lógica de Control
1. **Control de Válvulas:**
   - Válvula de Entrada (VE):
     - Abre ('A') cuando nivel < consignaMin
     - Cierra ('C') cuando nivel > consignaMax
   - Válvula de Salida (VS):
     - Abre ('A') cuando nivel > consignaMax
     - Cierra ('C') cuando nivel < consignaMin

2. **Estados del Sistema:**
   - **Modo Normal:** Funcionamiento estándar
   - **Modo Alarma:** Activado en condiciones anómalas

### Interfaz de Operador
- **LCD Display:**
  - Muestra estado de válvulas
  - Nivel actual
  - Temperatura
  - Periodo de muestreo

- **Terminal UART:**
  - Consigna actual
  - Nivel del depósito
  - Estado de válvulas
  - Temperatura
  - Estado de alarma
  - Configuración de periodos

### Comandos del Operador
- **Cambio de Consigna:**
  - 'a': 500mm
  - 'b': 750mm
  - 'c': 1000mm
  - 'd': 1250mm

- **Ajuste de Periodo:**
  - 'h': 10ms
  - 'j': 20ms
  - 'k': 50ms
  - 'l': 100ms

## Características Técnicas

### Hardware
- Arduino Mega 2560
- Módulo CAN Bus
- Sensores de nivel
- Válvulas de control
- Display LCD
- Terminal UART

### Software
- Comunicación CAN a 500kbps
- Control de temporizadores
- Gestión de interrupciones
- Protocolo de comunicación master-slave
- Interfaz de usuario 