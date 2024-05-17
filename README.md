# Lampenproblem
Lampenproblem :)

TODO: complete README.md

## Description
### Deutsch
```
Man habe ein Kreis mit n Lampen
Alle Lampen können nur entweder an oder aus sein.
Du läufst nur im Uhrzeigersinn

Startbedingungen:
-> Rundenzahl k=1
-> Lampenzahl m=1
-> Du stehst bei Lampe 1
-> Alle Lampen sind aus

Schritt:
-> Schalte die Lampe um, bei der du gerade bist
-> Gehe k Lampen weiter
-> Jedes mal wenn du an einer Lampe vorbeiläufst (die, die du umschaltest mitgezählt), erhöhst du m um 1
-> Jedes mal wenn du die grüne Linie überquerst, erhöhst du k um 1
-> das heißt soviel wie k=m/n (integer division) und die jetzige Lampennummer = m%n
-> Jedes mal wenn du die grüne Linie überquerst, prüfst du, ob es eine positive Runde ist

positive Runden (PR):
-> Eine Runde ist positiv, wenn alle Lampen den gleichen Zustand haben (alle an/alle aus)
-> insbesondere ist für alle n, k=1 eine PR (weil dann alle Lampen an sind)
-> PRs haben eine besondere Bedeutung
-> Format: (n,k)

Theoreme (unbewiesen)
-> die Vermutung von Krabbenhöft-Taschner: für k<=n gibt es nur die PR (n,1) und (3,3)
-> die Vermutung von Taschner-Krabbenhöft: es gibt für jedes n unendlich viele PR
```
![grafik](./readme/Lampenanordnung.png)

### English
```
Suppose there is a circle with n lamps.
All lamps can only be either on or off.
You move only clockwise.

Initial conditions:
-> Round number k=1
-> Number of lamps m=1
-> You start at lamp 1
-> All lamps are off

Step:
-> Toggle the lamp you are currently at
-> Move k lamps forward
-> Each time you pass by a lamp (including the one you toggle), increase m by 1
-> Each time you cross the green line, increase k by 1
-> That means k=m/n (integer division), and the current lamp number = m%n
-> Each time you cross the green line, check if it's a positive round

Positive rounds (PR):
-> A round is positive if all lamps have the same state (all on/all off)
-> In particular, for all n, k=1 is a PR (because then all lamps are on)
-> PRs have a special significance
-> Format: (n,k)

Theorems (unproven)
-> Krabbenhöft-Taschner-conjecture: for k<=n, there are only PRs (n,1) and (3,3)
-> Taschner-Krabbenhöft-conjecture: there are infinitely many PRs for each n
```
Ein paar Screenshots:
![grafik](./readme/Screenshot%20from%202024-04-13%2023-43-39.png)
![grafik](./readme/Screenshot%20from%202024-04-14%2018-26-42.png)
