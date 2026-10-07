While this was the initial circuit, it did not ground the PTT or CW key. I will create V2. We added a 1k Resistor and a 2N2222
~~~
ESP32 GPIO 26
      │
    330 Ω
      │
      │
 PC817 Pin 1
      │
   internal
     LED
      │
 PC817 Pin 2
      │
 ESP32 GND
~~~
~~~
                 PC817
           ┌─────────────┐

Pixie KEY ─── Pin 4
                 │
                 │ collector
                |/
                |
                |\
                 │ emitter
Pixie GND ─── Pin 3

           └─────────────┘
~~~
~~~
       3.5 mm TS plug

       TIP              SLEEVE
        │                  │
        ▼                  ▼
      ┌────┬────────────────────┐
      │    │                    │
      └────┴────────────────────┘

       KEY                 GND
        │                   │
   PC817 pin 4         PC817 pin 3
~~~
