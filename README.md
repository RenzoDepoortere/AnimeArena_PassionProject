# AnimeArena

Here I established the combat framework.
Since I wanted combat to be combo-based, I wanted to make it quite easy for myself to add in new attacks later.

I decided to have each attack-input be conversed into a string and store it.
Then I would identify which attack-string contained the stored string 
and execute the attack that coincided with the current attack index.

The attack contains an animation and some data (damage, knockback, ...).

![AttackString](https://github.com/RenzoDepoortere/AnimeArena/assets/95619804/ef592117-61c0-4aa9-a041-a1d0861f88d2)
