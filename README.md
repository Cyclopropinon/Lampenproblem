# Lampenproblem
Lampenproblem :)

TODO: complete README.md

```
Man habe ein Kreis mit n Lampen
Alle Lampen können nur entweder an oder aus sein.
Du läufst nur im Uhrzeigersinn

Startbedingungen:
-> Rundenzahl k=0
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
![grafik](https://github.com/Cyclopropinon/Lampenproblem/assets/119750500/b384c8f6-5ae9-4639-bb8d-0e3dbe831cc1)

