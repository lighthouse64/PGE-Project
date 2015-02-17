/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef LVL_PLAYER_H
#define LVL_PLAYER_H

#include "../../graphics/lvl_camera.h"
#include "lvl_warp.h"

#include "../../physics/base_object.h"
#include "../../controls/controllable_object.h"
#include <file_formats.h>
#include <QMap>

class LVL_Player :
        public PGE_Phys_Object,
        public ControllableObject
{
    public:
        LVL_Player();
        ~LVL_Player();
        void init();
        void update(float ticks=1.0f);

        int playerID;

        float32 force; //!< Move force
        float32 hMaxSpeed; //!< Max walk speed
        float32 hRunningMaxSpeed; //!< Max run speed
        float32 fallMaxSpeed; //!< Max fall speed

        float32 curHMaxSpeed; //!< Current moving max speed
        bool isRunning;

        bool JumpPressed;
        bool onGround;

        bool bumpDown;
        bool bumpUp;
        float bumpVelocity;
        void bump(bool _up=false);

        int foot_contacts;
        QMap<int, int > foot_contacts_map;
        int jumpForce;

        QMap<int, int > climbable_map;
        bool climbing;

        QMap<int, int > environments_map;
        int environment;
        int last_environment;

        bool isLive;
        enum deathReason
        {
            DEAD_fall=0,
            DEAD_burn,
            DEAD_killed
        };
        void kill(deathReason reason=DEAD_killed);
        bool doKill;
        deathReason kill_reason;

        int health;
        bool doHarm;
        int doHarm_damage;
        void harm(int _damage=1);

        bool contactedWithWarp;
        LVL_Warp * contactedWarp;
        bool wasTeleported;
        bool wasEntered;
        int warpsTouched;

        float32 gscale_Backup;

        bool   isWarping;
        bool   warpDo;
        int    warpDirect;
        uint32 warpWaitTicks;

        //floating
        bool allowFloat;
        bool isFloating;
        float timeToFloat;
        float maxFloatTime;

        PlayerPoint data;

        PGE_LevelCamera * camera;

        void teleport(float x, float y);
        void exitFromLevel(QString levelFile, int targetWarp);

        void render(float camX, float camY);
};

#endif // LVL_PLAYER_H
