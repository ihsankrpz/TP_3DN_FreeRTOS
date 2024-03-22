# TP_3DN_FreeRTOS

L’objectif de ce TP sur cinq séances est de mettre en place quelques applications
sous FreeRTOS en utilisant la carte STM32f746-Discovery conçue autour du
microcontrôleur STM32F746ng.

## FreeRTOS, tâches et sémaphores

Le paramètre TOTAL_HEAP_SIZE est important pour allouer le plus de place possible
dyanmiquement pour les variables.

Dans cette étape du TP nous en sommes rendu à créer des sémaphores et à jouer avec les priorités des tâches.
Nous avons deux tâches : task_give qui prend le sémaphore pour un délai de 100ms et 
task_take qui prend le sémaphore dès qu'il a l'occasion et le bloque.
Dans le cas où task_give est plus prioritaire, nous avons une sortie samblable à :

/*
Task give before
Task take before
Task take after
Task take before
Task give after
*/

Cela est dû au délai de 100ms. task_take à le temps de reprendre le sémaphore avant le "give after"

Lorsqu'on inverse les priorités; task_take plus prioritaire; nous avons : 

/*
Task take before
Task give before
Task take after
Task take before
Task give after
Task give before
Task take after
*/


