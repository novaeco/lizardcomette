#ifndef SCHEDULER_H
#define SCHEDULER_H

/**\brief Initialise le planificateur de t\xC3\xA2ches. */
void scheduler_init(void);
void scheduler_check_stock_levels(void);
void scheduler_check_regulatory_deadlines(void);

#endif // SCHEDULER_H
