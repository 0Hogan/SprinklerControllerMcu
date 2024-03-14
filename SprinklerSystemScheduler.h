#include <memory.h>

#include "SprinklerSystem.h"

#ifndef SPRINKLER_SYSTEM_SCHEDULER_H
#define SPRINKLER_SYSTEM_SCHEDULER_H

class SprinklerSystemScheduler
{
  public:
    SprinklerSystemScheduler(SprinklerSystem sprinklers) : m_sprinklers(sprinklers) {};



  private:
    SprinklerSystem m_sprinklers = nullptr;
};

#endif //SPRINKLER_SYSTEM_SCHEDULER_H