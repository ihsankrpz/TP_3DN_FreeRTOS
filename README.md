# TP_3DN_FreeRTOS

L’objectif de ce TP sur cinq séances est de mettre en place quelques applications
sous FreeRTOS en utilisant la carte STM32f746-Discovery conçue autour du
microcontrôleur STM32F746ng.

## FreeRTOS, tâches et sémaphores

Le paramètre TOTAL_HEAP_SIZE est important pour allouer le plus de place possible
dyanmiquement pour les variables.

Dans cette étape du TP nous en sommes rendu à créer des sémaphores et à jouer avec les priorités des tâches.
Nous avons deux tâches : task_give qui prend le sémaphore pour un délai de 100ms et task_take qui prend le sémaphore dès qu'il a l'occasion et le bloque.
Dans le cas où task_give est plus prioritaire, nous avons une sortie samblable à :

/*
Task give before
Task take before
Task take after
Task take before
Task give after
...
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
...
*/

La prochaine étape est de réaliser les tâches avec des notifications puis en utilisant une queue. (voir commit 36ac1130)
Pour les notifications, nous avons le même fonctionnement que pour le semaphore.

Le fonctionnement des queue est un peu différent. La queue met dans une fifo la valeur à envoyer et pour la réception, il faut lire la valeur.

/*
// send dans task1
q_value_send = i;
xQueueSend(QueueHandle, &q_value_send ,portMAX_DELAY);
vTaskDelay(portTICK_PERIOD_MS*100*i);

//receive dans task2
ret_q = xQueueReceive(QueueHandle, &q_value_receive ,1000);
if(ret_q != pdTRUE)
{
    NVIC_SystemReset();
    printf("System Reset\r\n");
}
printf("Received %d\r\n", q_value_receive);
*/


Pour la suite nous récupérons un projet avec un problème au lien : https://github.com/lfiack/tp_freertos_reentrance. 
Nous avons : 
#define TASK1_PRIORITY 1
#define TASK2_PRIORITY 2

#define TASK1_DELAY 1
#define TASK2_DELAY 2

et la sortie ressemble à : 
/*
Je suis la tache 2 et je m'endors pour 2 ticks
Je suis la tache 2 et je m'endors pour 2 ticks
Je suis la tache 1 et je m'endors pour 2 ticks
Je suis la tache 2 et je m'endors pour 2 ticks
Je suis la tache 2 et je m'endors pour 2 ticks
Je suis la tache 1 et je m'endors pour 2 ticks
...
*/

Le problème ici est que les deux tâches s'endors pour le même nombre de ticks alors que la tâche 1 doit s'endormir pour 1 tick.
Si on regarde le code, nous remarquons qu'ils accèdent au printf en même temps et donc influe sur la valeur. 
La tâche 2 étant initialisé en dernier, seul le TASK2_DELAY est pris en compte.
Pour résoudre le problème, il faut utiliser le sémaphore mutex. 

En rajoutant, une section critique autours des printf et en créeant cette fois-ci un semaphore mutex, nous avons : 

/*
Je suis la tache 2 et je m'endors pour 2 ticks
Je suis la tache 1 et je m'endors pour 1 ticks
Je suis la tache 2 et je m'endors pour 2 ticks
Je suis la tache 1 et je m'endors pour 1 ticks
Je suis la tache 2 et je m'endors pour 2 ticks
...
*/

Le problème est donc résolue.



