/*
Copyright (C) 2003, 2010 - Wolfire Games
Copyright (C) 2010-2017 - Lugaru contributors (see AUTHORS file)

This file is part of Lugaru.

Lugaru is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Lugaru is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Lugaru.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "Graphic/Sprite.hpp"

#include "Game.hpp"
#include "Objects/Person.hpp"

extern XYZ viewer;
extern float viewdistance;
extern float fadestart;
extern int environment;
extern float texscale;
extern Light light;
extern float multiplier;
extern float gravity;
extern Terrain terrain;
extern int detail;
extern XYZ viewerfacing;
extern int bloodtoggle;
extern XYZ windvector;

// init statics
Texture Sprite::cloudtexture;
Texture Sprite::cloudimpacttexture;
Texture Sprite::bloodtexture;
Texture Sprite::flametexture;
Texture Sprite::bloodflametexture;
Texture Sprite::smoketexture;
Texture Sprite::snowflaketexture;
Texture Sprite::shinetexture;
Texture Sprite::splintertexture;
Texture Sprite::leaftexture;
Texture Sprite::toothtexture;

float Sprite::checkdelay = 0;

vector<Sprite> Sprite::sprites = vector<Sprite>();

//Functions
void Sprite::Draw()
{
    int k;
    static float M[16];
    static XYZ point;
    static float distancemult;
    static int lasttype;
    static int lastspecial;
    static int whichpatchx, whichpatchz;
    static XYZ start, end, colpoint;
    static bool check;
    static bool blend;
    static float tempmult;
    static XYZ difference;
    static float lightcolor[3];
    static float viewdistsquared = viewdistance * viewdistance;
    static XYZ tempviewer;
    vector<Sprite>::iterator it;

    tempviewer = viewer + viewerfacing * 6;
    check = 0;

    lightcolor[0] = light.color[0] * .5 + light.ambient[0];
    lightcolor[1] = light.color[1] * .5 + light.ambient[1];
    lightcolor[2] = light.color[2] * .5 + light.ambient[2];

    checkdelay -= multiplier * 10;

    if (checkdelay <= 0) {
        check = 1;
        checkdelay = 1;
    }

    lasttype = -1;
    lastspecial = -1;
    glEnable(GL_BLEND);
    glDisable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);
    blend = 1;
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(0);
    glAlphaFunc(GL_GREATER, 0.0001);
    for (it = sprites.begin(); it != sprites.end(); it++) {
        if (lasttype != it->type) {
            switch (it->type) {
                case cloudsprite:
                    cloudtexture.bind();
                    if (!blend) {
                        blend = 1;
                        glAlphaFunc(GL_GREATER, 0.0001);
                        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                    }
                    break;
                case breathsprite:
                case cloudimpactsprite:
                    cloudimpacttexture.bind();
                    if (!blend) {
                        blend = 1;
                        glAlphaFunc(GL_GREATER, 0.0001);
                        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                    }
                    break;
                case smoketype:
                    smoketexture.bind();
                    if (!blend) {
                        blend = 1;
                        glAlphaFunc(GL_GREATER, 0.0001);
                        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                    }
                    break;
                case bloodsprite:
                    bloodtexture.bind();
                    if (!blend) {
                        blend = 1;
                        glAlphaFunc(GL_GREATER, 0.0001);
                        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                    }
                    break;
                case splintersprite:
                    if (lastspecial != it->special) {
                        if (it->special == 0) {
                            splintertexture.bind();
                        }
                        if (it->special == 1) {
                            leaftexture.bind();
                        }
                        if (it->special == 2) {
                            snowflaketexture.bind();
                        }
                        if (it->special == 3) {
                            toothtexture.bind();
                        }
                        if (!blend) {
                            blend = 1;
                            glAlphaFunc(GL_GREATER, 0.0001);
                            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                        }
                    }
                    break;
                case snowsprite:
                    snowflaketexture.bind();
                    if (!blend) {
                        blend = 1;
                        glAlphaFunc(GL_GREATER, 0.0001);
                        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                    }
                    break;
                case weaponshinesprite:
                    shinetexture.bind();
                    if (blend) {
                        blend = 0;
                        glAlphaFunc(GL_GREATER, 0.001);
                        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
                    }
                    break;
                case flamesprite:
                case weaponflamesprite:
                    flametexture.bind();
                    if (blend || lasttype == bloodflamesprite) {
                        blend = 0;
                        glAlphaFunc(GL_GREATER, 0.3);
                        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
                    }
                    break;
                case bloodflamesprite:
                    bloodflametexture.bind();
                    if (blend) {
                        blend = 0;
                        glAlphaFunc(GL_GREATER, 0.3);
                        glBlendFunc(GL_ONE, GL_ZERO);
                    }
                    break;
            }
        }
        if (it->type == snowsprite) {
            distancemult = (144 -
                            (distsq(&tempviewer, &it->position) -
                             (144 * fadestart)) *
                            (1 / (1 - fadestart))) / 144;
        } else {
            distancemult = (viewdistsquared -
                            (distsq(&viewer, &it->position) -
                             (viewdistsquared * fadestart)) *
                            (1 / (1 - fadestart))) / viewdistsquared;
        }
        if (it->type == flamesprite) {
            if (distancemult >= 1) {
                glColor4f(it->color[0],
                          it->color[1],
                          it->color[2],
                          it->opacity);
            } else {
                glColor4f(it->color[0],
                          it->color[1],
                          it->color[2],
                          it->opacity * distancemult);
            }
        } else {
            if (distancemult >= 1) {
                glColor4f(it->color[0] * lightcolor[0],
                          it->color[1] * lightcolor[1],
                          it->color[2] * lightcolor[2],
                          it->opacity);
            } else {
                glColor4f(it->color[0] * lightcolor[0],
                          it->color[1] * lightcolor[1],
                          it->color[2] * lightcolor[2],
                          it->opacity * distancemult);
            }
        }
        lasttype = it->type;
        lastspecial = it->special;
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glTranslatef(it->position.x, it->position.y, it->position.z);
        if ((it->type == flamesprite ||
             it->type == weaponflamesprite ||
             it->type == weaponshinesprite)) {
            difference = viewer - it->position;
            Normalise(&difference);
            glTranslatef(difference.x * it->size / 4,
                         difference.y * it->size / 4,
                         difference.z * it->size / 4);
        }
        if (it->type == snowsprite) {
            glRotatef(it->rotation * .2, 0, .3, 1);
            glTranslatef(1, 0, 0);
        }
        glGetFloatv(GL_MODELVIEW_MATRIX, M);
        point.x = M[12];
        point.y = M[13];
        point.z = M[14];
        glLoadIdentity();
        glTranslatef(point.x, point.y, point.z);

        glRotatef(it->rotation, 0, 0, 1);

        if ((it->type == flamesprite ||
             it->type == weaponflamesprite ||
             it->type == weaponshinesprite ||
             it->type == bloodflamesprite)) {
            if (it->alivetime < .14) {
                glScalef(it->alivetime / .14,
                         it->alivetime / .14,
                         it->alivetime / .14);
            }
        }
        if (it->type == smoketype ||
            it->type == snowsprite ||
            it->type == weaponshinesprite ||
            it->type == breathsprite) {
            if (it->alivetime < .3) {
                if (distancemult >= 1) {
                    glColor4f(it->color[0] * lightcolor[0],
                              it->color[1] * lightcolor[1],
                              it->color[2] * lightcolor[2],
                              it->opacity * it->alivetime / .3);
                }
                if (distancemult < 1) {
                    glColor4f(it->color[0] * lightcolor[0],
                              it->color[1] * lightcolor[1],
                              it->color[2] * lightcolor[2],
                              it->opacity * distancemult * it->alivetime / .3);
                }
            }
        }
        if (it->type == splintersprite &&
            it->special > 0 &&
            it->special != 3) {
            if (it->alivetime < .2) {
                if (distancemult >= 1) {
                    glColor4f(it->color[0] * lightcolor[0],
                              it->color[1] * lightcolor[1],
                              it->color[2] * lightcolor[2],
                              it->alivetime / .2);
                } else {
                    glColor4f(it->color[0] * lightcolor[0],
                              it->color[1] * lightcolor[1],
                              it->color[2] * lightcolor[2],
                              distancemult * it->alivetime / .2);
                }
            } else {
                if (distancemult >= 1) {
                    glColor4f(it->color[0] * lightcolor[0],
                              it->color[1] * lightcolor[1],
                              it->color[2] * lightcolor[2], 1);
                } else {
                    glColor4f(it->color[0] * lightcolor[0],
                              it->color[1] * lightcolor[1],
                              it->color[2] * lightcolor[2],
                              distancemult);
                }
            }
        }
        if (it->type == splintersprite &&
            (it->special == 0 || it->special == 3)) {
            if (distancemult >= 1) {
                glColor4f(it->color[0] * lightcolor[0],
                          it->color[1] * lightcolor[1],
                          it->color[2] * lightcolor[2], 1);
            } else {
                glColor4f(it->color[0] * lightcolor[0],
                          it->color[1] * lightcolor[1],
                          it->color[2] * lightcolor[2],
                          distancemult);
            }
        }

        glBegin(GL_TRIANGLES);
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(.5 * it->size, .5 * it->size, 0.0f);
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(-.5 * it->size, .5 * it->size, 0.0f);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(.5 * it->size, -.5 * it->size, 0.0f);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(-.5 * it->size, -.5 * it->size, 0.0f);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(.5 * it->size, -.5 * it->size, 0.0f);
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(-.5 * it->size, .5 * it->size, 0.0f);
        glEnd();
        glPopMatrix();
    }
    tempmult = multiplier;

    it = sprites.begin();
    while (it != sprites.end()) {
        bool spritehit = 0;
        multiplier = tempmult;
        if (it->type != snowsprite) {
            it->position += it->velocity * multiplier;
            it->velocity += windvector * multiplier;
        }

        if (it->type == flamesprite || it->type == smoketype) {
            it->position += windvector * multiplier / 2;
        }

        if ((it->type == flamesprite ||
             it->type == weaponflamesprite ||
             it->type == weaponshinesprite ||
             it->type == bloodflamesprite)) {
            multiplier *= it->speed * .7;
        }
        it->alivetime += multiplier;

        if (it->type == cloudsprite || it->type == cloudimpactsprite) {
            it->opacity -= multiplier / 2;
            it->size += multiplier / 2;
            it->velocity.y += gravity * multiplier * .25;
        }

        else if (it->type == breathsprite) {
            it->opacity -= multiplier / 2;
            it->size += multiplier / 2;
            if (findLength(&it->velocity) <= multiplier) {
                it->velocity = 0;
            } else {
                XYZ slowdown;
                slowdown = it->velocity * -1;
                Normalise(&slowdown);
                slowdown *= multiplier;
                it->velocity += slowdown;
            }
        }

        else if (it->type == snowsprite) {
            it->size -= multiplier / 120;
            it->rotation += multiplier * 360;
            it->position.y -= multiplier;
            it->position += windvector * multiplier;
            if (it->position.y < tempviewer.y - 6) {
                it->position.y += 12;
            }
            if (it->position.y > tempviewer.y + 6) {
                it->position.y -= 12;
            }
            if (it->position.z < tempviewer.z - 6) {
                it->position.z += 12;
            }
            if (it->position.z > tempviewer.z + 6) {
                it->position.z -= 12;
            }
            if (it->position.x < tempviewer.x - 6) {
                it->position.x += 12;
            }
            if (it->position.x > tempviewer.x + 6) {
                it->position.x -= 12;
            }
        }

        else if (it->type == bloodsprite) {
            it->rotation += multiplier * 100;
            it->velocity.y += gravity * multiplier;
            if (check) {
                XYZ where, startpoint, endpoint, movepoint, footpoint;
                float rotationpoint;
                int whichtri;

                for (unsigned j = 0; j < Person::players.size(); j++) {
                    if (Person::players[j]->dead && it->alivetime > .1) {
                        where = it->oldposition;
                        where -= Person::players[j]->coords;
                        if (!Person::players[j]->skeleton.free) {
                            where = DoRotation(where, 0, -Person::players[j]->yaw, 0);
                        }
                        startpoint = where;
                        where = it->position;
                        where -= Person::players[j]->coords;
                        if (!Person::players[j]->skeleton.free) {
                            where = DoRotation(where, 0, -Person::players[j]->yaw, 0);
                        }
                        endpoint = where;

                        movepoint = 0;
                        rotationpoint = 0;
                        whichtri = Person::players[j]->skeleton.drawmodel
                            .LineCheck(&startpoint, &endpoint, &footpoint, &movepoint, &rotationpoint);
                        if (whichtri != -1) {
                            spritehit = 1;
                            Person::players[j]->DoBloodBigWhere(0, 160, it->oldposition);
                            it = sprites.erase (it);
                            break;
                        }
                    }
                }

                if (!spritehit) {
                    whichpatchx = it->position.x / (terrain.size / subdivision * terrain.scale);
                    whichpatchz = it->position.z / (terrain.size / subdivision * terrain.scale);
                    if (whichpatchx > 0 &&
                        whichpatchz > 0 &&
                        whichpatchx < subdivision &&
                        whichpatchz < subdivision) {
                        unsigned int j;
                        for (j = 0; j < terrain.patchobjects[whichpatchx][whichpatchz].size(); j++) {
                            k = terrain.patchobjects[whichpatchx][whichpatchz][j];
                            start = it->oldposition;
                            end = it->position;
                            if (Object::objects[k]->model
                                .LineCheck(&start, &end, &colpoint,
                                           &Object::objects[k]->position,
                                           &Object::objects[k]->yaw) != -1) {
                                if (detail == 2 ||
                                    (detail == 1 && abs(Random() % 4) == 0) ||
                                    (detail == 0 && abs(Random() % 8) == 0)) {
                                    Object::objects[k]->model
                                        .MakeDecal(blooddecalfast,
                                                   DoRotation(colpoint -
                                                              Object::objects[k]->position, 0,
                                                              -Object::objects[k]->yaw, 0),
                                                   it->size * 1.6, .5, Random() % 360);
                                }
                                it = sprites.erase (it);
                                spritehit = 1;
                                break;
                            }
                        }
                    }
                }

                if (!spritehit &&
                    it->position.y < terrain.getHeight(it->position.x,
                                                       it->position.z)) {
                    terrain.MakeDecal(blooddecalfast, it->position,
                                      it->size * 1.6, .6, Random() % 360);
                    it = sprites.erase (it);
                    spritehit = 1;
                }
            }
        }

        else if (it->type == splintersprite) {
            it->rotation += it->rotatespeed * multiplier;
            it->opacity -= multiplier / 2;
            if (it->special == 0 || it->special == 2 || it->special == 3) {
                it->velocity.y += gravity * multiplier;
            }
            if (it->special == 1) {
                it->velocity.y += gravity * multiplier * .5;
            }
        }

        else if (it->type == flamesprite ||
            it->type == weaponflamesprite ||
            it->type == weaponshinesprite ||
            it->type == bloodflamesprite) {
            it->rotation += multiplier * it->rotatespeed;
            it->opacity -= multiplier * 5 / 4;
            if (it->type != weaponshinesprite && it->type != bloodflamesprite) {
                if (it->opacity < .5 &&
                    it->opacity + multiplier * 5 / 4 >= .5 &&
                    (abs(Random() % 4) == 0 || (it->initialsize > 2 && Random() % 2 == 0))) {
                    MakeSprite(smoketype, it->position,
                               it->velocity, .9, .9, .6, it->size * 1.2, .4);
                }
            }
            if (it->alivetime > .14 && (it->type == flamesprite)) {
                it->velocity = 0;
                it->velocity.y = 1.5;
            }
        }

        else if (it->type == smoketype) {
            it->opacity -= multiplier / 3 / it->initialsize;
            it->color[0] -= multiplier;
            it->color[1] -= multiplier;
            it->color[2] -= multiplier;
            if (it->color[0] < .6) {
                it->color[0] = .6;
            }
            if (it->color[1] < .6) {
                it->color[1] = .6;
            }
            if (it->color[2] < .6) {
                it->color[2] = .6;
            }
            it->size += multiplier;
            it->velocity = 0;
            it->velocity.y = 1.5;
            it->rotation += multiplier * it->rotatespeed / 5;
        }

        if (!spritehit) {
            if (it->opacity <= 0 || it->size <= 0) {
                it = sprites.erase (it);
            } else {
                it++;
            }
        }
    }
    if (check) {
        for (it = sprites.begin(); it != sprites.end(); it++) {
            it->oldposition = it->position;
        }
    }
    glAlphaFunc(GL_GREATER, 0.0001);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Sprite::MakeSprite(int atype, XYZ where, XYZ avelocity, float red, float green, float blue, float asize, float aopacity)
{
    if (sprites.size() < max_sprites - 1) {
        Sprite new_sprite;
        if ((atype != bloodsprite && atype != bloodflamesprite) || bloodtoggle) {
            new_sprite.special = 0;
            new_sprite.type = atype;
            new_sprite.position = where;
            new_sprite.oldposition = where;
            new_sprite.velocity = avelocity;
            new_sprite.alivetime = 0;
            new_sprite.opacity = aopacity;
            new_sprite.size = asize;
            new_sprite.initialsize = asize;
            new_sprite.color[0] = red;
            new_sprite.color[1] = green;
            new_sprite.color[2] = blue;
            new_sprite.rotatespeed = abs(Random() % 720) - 360;
            new_sprite.speed = float(abs(Random() % 100)) / 200 + 1.5;
        }
        sprites.push_back(new_sprite);
    }
}

Sprite::Sprite()
{
    oldposition = 0;
    position = 0;
    velocity = 0;
    size = 0;
    initialsize = 0;
    type = 0;
    special = 0;
    memset(color, 0, sizeof(color));
    opacity = 0;
    rotation = 0;
    alivetime = 0;
    speed = 0;
    rotatespeed = 0;
}
