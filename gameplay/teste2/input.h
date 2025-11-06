#ifndef INPUT_H
#define INPUT_H

#include "defs.h"

// Nota: Removi 'bullets' daqui. O tiro por 'ESPAÇO' era redundante
// com o tiro automático na função update(). Se quiser manter,
// adicione 'Bullet bullets[]' de volta.
void handleInput(Player *player, bool *running);

#endif // INPUT_H
