# SMARTHOME Projects with ESPHome

Last update: 04. 04. 2023

# Table of contents
- [Project descriptons](#project_descriptons)
  - [Thermostat with NEXTION](#thermostat-with-nextion)
  - [GAS Metering with simple REED switch](#gas-metering-with-simple-reed-switch)
  - [RGB LED strip driver with ULN2003A](#rgb-led-strip-driver-with-uln2003a)
  - [Survillance CAM with ESP32-CAM](#survillance-cam-with-esp32-cam)
- [Prerequisite](#prerequisite)
- [Helpers](#helpers)
  - [ESPHome Default configs](#esphome-default-configs)
    - [Board naming convention](#Board-naming-convention)
      - [Board types](#Board-types)
      - [Group types](#Group-types)
    - [YAML structure](#yaml-structure)
    - [MQTT naming convention and the JSON structure](#mqtt-naming-convention-and-the-json-structure)
  - [Changing params](#changing-params)
    - [Device name - change](#device-name---change)
    - [API - Update encryption key](#api---update-encryption-key)
    - [OTA - Updating the password:](#ota---updating-the-password)

## Project descriptons
### Thermostat with NEXTION
#### Description:
#### Releases:

> #### 0.3.1

Codeblocks for 
* esphome
```
esphome:
  # Project/Board definition
  <<: !include includes/base/.base.esphome-definition.yaml
  # Board type
  <<: !include includes/boards/.board.nodemcu-32s.yaml
  project:
    <<: !include projects/livingroom-thermostat.yaml
```
* button: restart, restart with defaults
```
button:
  <<: !include includes/components/.components.button.restart.yaml
  <<: !include includes/components/.components.button.restart-factory.yaml
```
* components topics
```
<<: !include { file: includes/mqtt/.mqtt.components-topic-with-command.yaml, vars: { component_type: "switch", component_id: "internal_led" } } # inline syntax
<<: !include { file: includes/mqtt/.mqtt.components-topic.yaml, vars: { component_type: "switch", component_id: "internal_led" } } # inline syntax
```
* sensor:
```
<<: !include { file: includes/mqtt/.mqtt.components-topic-with-command.yaml, vars: { component_type: "switch", component_id: "internal_led" } } # inline syntax
```
> #### 0.2.0

> #### 0.1.0

### GAS Metering with simple REED switch
#### Description:
#### Releases:
### RGB LED strip driver with ULN2003A
#### Description:
#### Releases:
### Survillance CAM with ESP32-CAM
#### Description:
#### Releases:

## Prerequisite

## Helpers
### ESPHome Default configs
#### Board naming convention

The main Goal is to clearly identify the unit **Group, Type and Location**. 
> Note: In the MQTT naming convention and the JSON structure Section can the JSON structure find, where all the nessesarry params

```
friendly_name: ${device_type} ${device_subtype} ${device_number}
```

```
# device naming
  device_group: Climate               # HA Card type
  device_type: Thermostat             # What device unit is? 
  device_subtype: Relay               # forex Thermostat system stands from Central Unit and a Relay / Relays
  device_number: "1"                  # each has own uniqe number
  device_name: thermostat-relay-1     # the device name
  
  # where the device is located
  device_location: Livingroom         # Location / room
  device_floor: Ground floor          # floor
```

##### Board types
* Thermostat
* LED-strip
* Camera
* Sensor

##### Group types
* Climate
* Lighting
* Metering
* Survillance

### YAML structure
Base configuration
It starts with the base configuration, which contains settings for all my ESPHome devices, regardless of their type, function, or brand.

> IMPORTANT: **TAB** ident 

![File structure](./images/esphome_config_structure.png)

1. Substitutions Section

This has more subsections as follows:
```
substitutions:
  # device naming
  device_group: Climate               # HA Card type
  device_type: Thermostat             # What device unit is? 
  device_subtype: Relay               # forex Thermostat system stands from Central Unit and a Relay / Relays
  device_number: "1"                  # each has own uniqe number
  device_name: thermostat-relay-1     # the device name

  # where the device is located
  device_location: Livingroom         # Location / room
  device_floor: Ground floor          # floor

  # which units are controlled
  unit_name: relay                    # not ncessarry
  parent_device: "Central_1"          # Who controls me / Who is my owner
  child_devices: "{Heater_1}"         # in HA switch to turn ON/OFF relay controlled unit (boiler)
  controlled_device: heater           # not used

  # other substitutions
  <<: !include .base.substitutions.yaml

  # MQTT device specific topics
  <<: !include .mqtt.default-topics.yaml
  # MQTT device specific topics

  # Other function specific includes forex.:
  # Homeassistant entities for temperature
  <<: !include .base.ha-entities-temperature.yaml
```
> IMPORTANT: The ***.base.substitutions.yaml** file has an includ, the **.base.mqtt-topic-prefix.yaml** file, which contains the following project specific naming for mqtt:
```
  # MQTT topic prefix forex
  mqtt_prefix: "smarthome_test/${device_group}/"
```

2. Esphome definition Section
```
esphome:
  name: ${device_name}
  friendly_name: ${device_type} ${device_subtype} ${device_number}
  comment: ${device_type} ${device_subtype} ${device_number} on Wemos D1 mini Pro with OLED in ${device_location} on ${device_floor}

  <<: !include .board.wemos-d1-mini-pro.yaml
```

Where each board type has its own definition file, like **.board.wemos-d1-mini-pro.yaml**

```
  board: d1_mini_pro
  platform: ESP8266
```

3. Connection Section
The **.base.connection-settings.yaml** file that contains the configuration for my WiFi network, fallback access point, API for Home Assistant, and over-the-air updates, looks like this:
```
# Enable logging
logger:

captive_portal:

    # Enable Home Assistant API
api:
  encryption:
    key: !secret encryption_key
  #encryption:
  #  key: "vy4v4mn8ktCeSRT9vpTcAG0YBiFiOi4Ao0J7FTgFrrM="

ota:
  password: !secret ota_password
    
wifi:
  ssid: !secret wifi_ssid
  password: !secret wifi_password
  #use_address: livingroom-thermostat.local

  ap:
    ssid: "$hot_spot"
    password: !secret ap_password
```

4. MQTT Section

The **.base.mqtt-connection.params.yaml** contains the following:
```
  broker: !secret mqtt_broker
  port: 1883
  username: !secret mqtt_username
  password: !secret mqtt_password
```
and than
```
# MQTT
mqtt:
  <<: !include .base.mqtt-connection-params.yaml
  
  on_connect:
    - wait_until: time.has_time #mqtt.connected
    - switch.turn_off: Heater_1           # device specific
    - mqtt.publish_json:
        topic: ${topic_state}
        payload: |-
          root["sender"] = "${device_type}_${device_subtype}_${device_number}";
          root["room"] = "${device_location}";
          root["floor"] = "${device_floor}";
          root["payload"]["state"] = "ON";
          root["trigger"] = "on_connect";
          root["timestamp"] = id(homeassistant_time).now().timestamp;
    - lambda: |- 
        id(mqtt_connection_state) = true;
    - logger.log: "${device_type} ${device_subtype} ${device_number} connected"
  on_disconnect:
    - switch.turn_off: Heater_1           # device specific
    - mqtt.publish_json:
        topic: ${topic_state}
        payload: |-
          root["sender"] = "${device_type}_${device_subtype}_${device_number}";
          root["room"] = "${device_location}";
          root["floor"] = "${device_floor}";
          root["payload"]["state"] = "OFF";
          root["trigger"] = "on_disconnect";
          root["timestamp"] = id(homeassistant_time).now().timestamp;
    - logger.log: "${device_type} ${device_subtype} ${device_number} disconnected"

  # Livingroom/Thermostat/Control-1/action
  # {sender: "Thermostat_Control_1", receiver: "Thermostat_Relay_1", room: "Livingroom", floor:"Ground floor", payload: {action: "ON"}}
  on_json_message:
    - topic: ${topic_action_receiver}
      then:
        - lambda: |-
            if (x.containsKey("receiver") && x["receiver"] == "${device_type}_${device_subtype}_${device_number}") {
              if (x["payload"]["action"] == "ON" ) {
                id(unit_state) = true;    
                id(Heater_1).turn_on();   # device specific
                
              } else {
                id(unit_state) = false;
                id(Heater_1).turn_off();  # device specific
              }
            }
    - topic: ${topic_state}
      then:
        - lambda: |-
            if (x.containsKey("receiver") && x["receiver"] == "${device_type}_${device_subtype}_${device_number}") {
              if (x["payload"]["state"] == "ON" ) {
                id(unit_state) = true;
                id(Heater_1).turn_on();   # device specific
              } else {
                id(unit_state) = false;
                id(Heater_1).turn_off();  # device specific
              }
            }
```
5. Globals & Font Section

```
globals:
  - id: unit_state
    type: bool
    restore_value: true
    initial_value: 'false'

...

font:
  - file: 'Roboto-Light.ttf'
    id: font1_1
    size: 8
...
```

6. HA Time Section

The **.base.ha-get-time.yaml*** contains the following:
```
# Current date and time
time:
  - platform: homeassistant
    id: homeassistant_time
```
and when you want to put into mqtt message:
```
...
  root["timestamp"] = id(homeassistant_time).now().timestamp
...
```

7. Device functionality code Section


## MQTT naming convention and the JSON structure


We include the **.base.mqtt-topic-prefix.yaml** to the **.base.substituions.yaml** file
```
  # MQTT topic prefix for testing
  mqtt_prefix: "smarthome_test/${device_group}/"
```
and define the device specific topics in the **.mqtt.thermostat-topics.yaml** as follows
```
${device_location}/${device_type}/${device_subtype}_${device_number}/
```
The topics naming
```
  # MQTT topics - Published
  topic_availability: ${device_location}/${device_type}/${device_subtype}_${device_number}/availability
  topic_state: ${device_location}/${device_type}/${device_subtype}_${device_number}/state
  topic_message: ${device_location}/${device_type}/${device_subtype}_${device_number}/message
  topic_action_sender: ${device_location}/${device_type}/ # ${child_devices[0]}/action

  # MQTT topics - Subscribed
  topic_action_receiver: ${device_location}/${device_type}/${parent_device}/action
```


```
        - mqtt.publish_json:
            topic: ${topic_message}
            payload: !lambda |-
              root["sender"] = "${device_type}_${device_subtype}_${device_number}";
              root["room"] = "${device_location}";
              root["floor"] = "${device_floor}";
              root["payload"]["measures"]["humidity"] = id(room_humidity).state;
              root["timestamp"] = id(homeassistant_time).now().timestamp;
```

# Changing params
## Device name - change
Changing an existing device name:
First change device name
```
substitutions:
  ...
  device_name: new-device-name

esphome:
  name: ${device_name}
```
then add the following line in WiFi block
```
wifi:
  ssid: !secret wifi_ssid
  password: !secret wifi_password
  use_address: old-hostname.local
```
Install and then remove the
```
use_address: old-hostname.local
```
and install again.

## API - Update encryption key
API transport encryption is now enabled by default when you create a new device in ESPHome. This will autogenerate a random encryption key in the device YAML file that you will need to retrieve when you attempt to add the device to Home Assistant. Simply remove the encryption key lines from your YAML should you choose to not use encryption.

First 
```
```
## OTA - Updating the password:
Changing an existing password:
Since the password is used both for compiling and uploading the regular esphome <file> run won’t work of course. This issue can be worked around by executing the operations separately through an on_boot trigger:
```
esphome:
  on_boot:
    - lambda: |-
        id(my_ota).set_auth_password("New password");
ota:
  password: "Old password"
  id: my_ota
```

