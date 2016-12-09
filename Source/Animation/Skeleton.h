/*
Copyright (C) 2003, 2010 - Wolfire Games
Copyright (C) 2010-2016 - Lugaru contributors (see AUTHORS file)

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

#ifndef _SKELETON_H_
#define _SKELETON_H_

#include "Models.h"
#include "Quaternions.h"


/**> HEADER FILES <**/
#include "gamegl.h"
#include "Quaternions.h"
#include "Objects.h"
#include "Sprite.h"
#include "binio.h"
#include "Animation/Animation.h"

enum bodyparts {
    head, neck,
    leftshoulder,  leftelbow,  leftwrist,  lefthand,
    rightshoulder, rightelbow, rightwrist, righthand,
    abdomen, lefthip, righthip, groin,
    leftknee,  leftankle,  leftfoot,
    rightknee, rightankle, rightfoot
};

class Joint
{
public:
    XYZ position;
    XYZ oldposition;
    XYZ realoldposition;
    XYZ velocity;
    XYZ oldvelocity;
    XYZ startpos;
    float blurred;
    float length;
    float mass;
    bool lower;
    bool hasparent;
    bool locked;
    int modelnum;
    bool visible;
    Joint* parent;
    bool sametwist;
    int label;
    int hasgun;
    float delay;
    XYZ velchange;

    Joint() {
        blurred = 0;
        length = 0;
        mass = 0;
        lower = 0;
        hasparent = 0;
        locked = 0;
        modelnum = 0;
        visible = 0;
        parent = 0;
        sametwist = 0;
        label = 0;
        hasgun = 0;
        delay = 0;
    }
};

class Muscle
{
public:
    int numvertices;
    int* vertices;
    int numverticeslow;
    int* verticeslow;
    int numverticesclothes;
    int* verticesclothes;
    float length;
    float targetlength;
    Joint* parent1;
    Joint* parent2;
    float maxlength;
    float minlength;
    int type;
    bool visible;
    float rotate1, rotate2, rotate3;
    float lastrotate1, lastrotate2, lastrotate3;
    float oldrotate1, oldrotate2, oldrotate3;
    float newrotate1, newrotate2, newrotate3;

    float strength;

    Muscle();
    ~Muscle();
    void DoConstraint(bool spinny);
};

const int max_joints = 50;

class Skeleton
{
public:
    int num_joints;
    //Joint joints[max_joints];
    //Joint *joints;
    Joint* joints;

    int num_muscles;
    //Muscle muscles[max_muscles];
    //Muscle *muscles;
    Muscle* muscles;

    int selected;

    int forwardjoints[3];
    XYZ forward;

    int id;

    int lowforwardjoints[3];
    XYZ lowforward;

    XYZ specialforward[5];
    int jointlabels[max_joints];

    Model model[7];
    Model modellow;
    Model modelclothes;
    int num_models;

    Model drawmodel;
    Model drawmodellow;
    Model drawmodelclothes;

    bool clothes;
    bool spinny;

    GLubyte skinText[512 * 512 * 3];
    int skinsize;

    float checkdelay;

    float longdead;
    bool broken;

    int free;
    int oldfree;
    float freetime;
    bool freefall;

    void FindForwards();
    float DoConstraints(XYZ *coords, float *scale);
    void DoGravity(float *scale);
    void FindRotationJoint(int which);
    void FindRotationJointSameTwist(int which);
    void FindRotationMuscle(int which, int animation);
    void Load(const std::string& fileName, const std::string& lowfileName, const std::string& clothesfileName, const std::string& modelfileName, const std::string& model2fileName, const std::string& model3fileName, const std::string& model4fileName, const std::string& model5fileNamee, const std::string& model6fileName, const std::string& model7fileName, const std::string& modellowfileName, const std::string& modelclothesfileName, bool aclothes);

    /*
    // unused
    void FindForwardsfirst();
    void Draw(int muscleview);
    void AddJoint(float x, float y, float z, int which);
    void SetJoint(float x, float y, float z, int which, int whichjoint);
    void DeleteJoint(int whichjoint);
    void AddMuscle(int attach1, int attach2, float maxlength, float minlength, int type);
    void DeleteMuscle(int whichmuscle);
    void DoBalance();
    void MusclesSet();
    */

    Skeleton();
    ~Skeleton();

private:
    // convenience functions
    // only for Skeleton.cpp
    inline Joint& joint(int bodypart) { return joints[jointlabels[bodypart]]; }
    inline XYZ& jointPos(int bodypart) { return joint(bodypart).position; }
    inline XYZ& jointVel(int bodypart) { return joint(bodypart).velocity; }

};

#endif