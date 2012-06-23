#pragma  once

#include "NxPhysics.h"
#include <stdio.h>
#include "NxStream.h"
#include "NxCooking.h"
#include "PhysxManager.h"
#include "Ogre.h"

using namespace Ogre;

class MyRagdoll
{
    NxScene *gScene;
    NxPhysicsSDK *gPhysicsSDK;
    PhysxManager *gPxMgr;

  public:

    NxActor* head;
    NxActor* neck;
    NxActor* spine2;
    NxActor* spine1;
    NxActor* spine;
    NxActor* pelvis;
    NxActor* leftUpperArm;
    NxActor* rightUpperArm;
    NxActor* leftForeArm;
    NxActor* rightForeArm;
    NxActor* leftHand;
    NxActor* rightHand;
    NxActor* leftThigh;
    NxActor* rightThigh;
    NxActor* leftCalf;
    NxActor* rightCalf;
    NxActor* leftFoot;
    NxActor* rightFoot;

    NxSphericalJoint* jNeck;
    NxSphericalJoint* jSpine2;
    NxSphericalJoint* jSpine1;
    NxSphericalJoint* jSpine;
    NxSphericalJoint* leftShoulder;
    NxSphericalJoint* rightShoulder;
    NxSphericalJoint* jLeftThigh;
    NxSphericalJoint* jRightThigh;
    NxSphericalJoint* jLeftCalf;
    NxSphericalJoint* jRightCalf;

    NxRevoluteJoint* leftElbow;
    NxRevoluteJoint* rightElbow;
    NxRevoluteJoint* leftWrist;
    NxRevoluteJoint* rightWrist;
    NxRevoluteJoint* leftKnee;
    NxRevoluteJoint* rightKnee;
    NxRevoluteJoint* leftAnkle;
    NxRevoluteJoint* rightAnkle;

    MyRagdoll(NxScene* scene, NxPhysicsSDK* sdk, Vector3 pos, Quaternion orient, SkeletonInstance *skeleton, PhysxManager *pxmgr)
    {
      gPhysicsSDK = sdk;
      gScene = scene;
      gPxMgr = pxmgr;
      NxQuat qRotLeft, qRotRight, qRotAround;
      qRotLeft.fromAngleAxis(90, NxVec3(0, 0, 1));
      qRotRight.fromAngleAxis(-90, NxVec3(0, 0, 1));
      qRotAround.fromAngleAxis(180, NxVec3(0, 0, 1));
      NxMat33 mRotLeft, mRotRight, mRotAround;
      mRotLeft.fromQuat(qRotLeft);
      mRotRight.fromQuat(qRotRight);
      mRotAround.fromQuat(qRotAround);
      //щитаем положение Джоинтов
      NxVec3 headPos = gPxMgr->toNx(orient * skeleton->getBone("Bip01 Head")->_getDerivedPosition() * 10 + pos);
      NxVec3 neckPos = gPxMgr->toNx(orient * skeleton->getBone("Bip01 Neck")->_getDerivedPosition() * 10 + pos);
      NxVec3 spine2Pos = gPxMgr->toNx(orient * skeleton->getBone("Bip01 Spine2")->_getDerivedPosition() * 10 + pos);
      NxVec3 spine1Pos = gPxMgr->toNx(orient * skeleton->getBone("Bip01 Spine1")->_getDerivedPosition() * 10 + pos);
      NxVec3 spinePos = gPxMgr->toNx(orient * skeleton->getBone("Bip01 Spine")->_getDerivedPosition() * 10 + pos);
      NxVec3 pelvisPos = gPxMgr->toNx(orient * skeleton->getBone("Bip01 Pelvis")->_getDerivedPosition() * 10 + pos);
      NxVec3 lThighPos = gPxMgr->toNx(orient * skeleton->getBone("Bip01 L Thigh")->_getDerivedPosition() * 10 + pos);
      NxVec3 rThighPos = gPxMgr->toNx(orient * skeleton->getBone("Bip01 R Thigh")->_getDerivedPosition() * 10 + pos);
      NxVec3 lCalfPos = gPxMgr->toNx(orient * skeleton->getBone("Bip01 L Calf")->_getDerivedPosition() * 10 + pos);
      NxVec3 rCalfPos = gPxMgr->toNx(orient * skeleton->getBone("Bip01 R Calf")->_getDerivedPosition() * 10 + pos);
      NxVec3 lFootPos = gPxMgr->toNx(orient * skeleton->getBone("Bip01 L Foot")->_getDerivedPosition() * 10 + pos);
      NxVec3 rFootPos = gPxMgr->toNx(orient * skeleton->getBone("Bip01 R Foot")->_getDerivedPosition() * 10 + pos);
      head = CreateSphere(headPos, 1, 10);
      neck = CreateSphere(NxVec3(neckPos.x, neckPos.y -= neckPos.distance(spine2Pos) / 2, neckPos.z), 1, 10);
      spine2 = CreateSphere(NxVec3(spine2Pos.x, spine2Pos.y -= spine2Pos.distance(spine1Pos) / 2, spine2Pos.z), 1, 10);
      spine1 = CreateSphere(NxVec3(spine1Pos.x, spine1Pos.y -= spine1Pos.distance(spinePos) / 2, spine1Pos.z), 1, 10);
      spine = CreateSphere(NxVec3(spinePos.x, spinePos.y -= spinePos.distance(pelvisPos) / 2, spinePos.z), 1, 10);
      leftCalf = CreateCapsule(NxVec3(lCalfPos.x, lCalfPos.y -= lCalfPos.distance(lThighPos) / 2, lCalfPos.z), lCalfPos.distance(lThighPos), 1, 10);
      rightCalf = CreateCapsule(NxVec3(rCalfPos.x, rCalfPos.y -= rCalfPos.distance(rThighPos) / 2, rCalfPos.z), rCalfPos.distance(rThighPos), 1, 10);
      leftFoot = CreateCapsule(NxVec3(lFootPos.x, lFootPos.y -= lFootPos.distance(lCalfPos) / 2, lFootPos.z), lFootPos.distance(lCalfPos), 1, 10);
      rightFoot = CreateCapsule(NxVec3(rFootPos.x, rFootPos.y -= rFootPos.distance(rCalfPos) / 2, rFootPos.z), rFootPos.distance(rCalfPos), 1, 10);
      //    NxVec3 lUpperArmPos = gPxMgr->toNx( orient*skeleton->getBone( "Bip01 L UpperArm")->_getDerivedPosition()*10+pos);
      //    leftUpperArm = CreateCapsule( lUpperArmPos, 1.5, 0.8, 10);
      //    mRotRight.fromQuat(qRotRight);
      //    leftUpperArm->setGlobalOrientationQuat( qRotAround);
      //    NxVec3 lForeArmPos = gPxMgr->toNx( orient*skeleton->getBone( "Bip01 L Hand")->_getDerivedPosition()*10+pos);
      //    leftForeArm = CreateCapsule( lForeArmPos, 4, 1, 10);
      //    leftForeArm->setGlobalOrientationQuat( qRotRight);
      //    NxVec3 lHandPos = gPxMgr->toNx( orient*skeleton->getBone( "Bip01 L Hand")->_getDerivedPosition()*10+pos);
      //    leftHand =  CreateCapsule( lHandPos, 4, 1, 10);
      //    leftHand->setGlobalOrientationQuat( qRotRight);
      /*
          rightUpperArm = CreateCapsule( gPxMgr->toNx( skeleton->getBone( "Bip01 R UpperArm")->_getDerivedPosition()), 1, 0.3, 10);
          rightUpperArm->setGlobalOrientationQuat(qRotLeft);
          rightForeArm = CreateCapsule( gPxMgr->toNx( skeleton->getBone( "Bip01 R Forearm")->_getDerivedPosition()), 1, 0.3, 10);
          rightForeArm->setGlobalOrientationQuat(qRotLeft);
          rightHand = CreateBox( gPxMgr->toNx( skeleton->getBone( "Bip01 R Hand")->_getDerivedPosition()), NxVec3( 0.3, 0.3, 0.1), 10);
          rightHand->setGlobalOrientationQuat(qRotLeft);

          leftThigh = CreateCapsule( gPxMgr->toNx( skeleton->getBone( "Bip01 L Thigh")->_getDerivedPosition()), 1.5, 0.5, 10);
          leftThigh->setGlobalOrientationQuat(qRotAround);
          leftCalf = CreateCapsule( gPxMgr->toNx( skeleton->getBone( "Bip01 L Calf")->_getDerivedPosition()), 1.5, 0.35, 10);
          leftCalf->setGlobalOrientationQuat(qRotAround);
          leftFoot = CreateBox( gPxMgr->toNx( skeleton->getBone( "Bip01 L Foot")->_getDerivedPosition()), NxVec3( 0.4, 0.2, 0.75), 10);
          leftFoot->setGlobalOrientationQuat(qRotAround);

          rightThigh = CreateCapsule( gPxMgr->toNx( skeleton->getBone( "Bip01 R Thigh")->_getDerivedPosition()), 1.5, 0.5, 10);
          rightThigh->setGlobalOrientationQuat(qRotAround);
          rightCalf = CreateCapsule( gPxMgr->toNx( skeleton->getBone( "Bip01 R Calf")->_getDerivedPosition()), 1.5, 0.35, 10);
          rightCalf->setGlobalOrientationQuat(qRotAround);
          rightFoot = CreateBox( gPxMgr->toNx( skeleton->getBone( "Bip01 R Foot")->_getDerivedPosition()), NxVec3( 0.4, 0.2, 0.75), 10);
          rightFoot->setGlobalOrientationQuat(qRotAround);
      */
      // создаём джоинты
      jNeck = CreateBodySphericalJoint(head, neck, neckPos, NxVec3(0, 1, 0));
      jSpine2 = CreateBodySphericalJoint(neck, spine2, spine2Pos, NxVec3(0, -1, 0));
      jSpine1 = CreateBodySphericalJoint(spine2, spine1, spine1Pos, NxVec3(0, -1, 0));
      jSpine = CreateBodySphericalJoint(spine1 , spine, spinePos, NxVec3(0, -1, 0));
      jLeftThigh = CreateBodySphericalJoint(spine, leftCalf, lThighPos,  NxVec3(1, 0, 0));
      jRightThigh = CreateBodySphericalJoint(spine, rightCalf, rThighPos,  NxVec3(1, 0, 0));
      jLeftCalf =  CreateBodySphericalJoint(leftCalf, leftFoot, lCalfPos, NxVec3(1, 0, 0));
      jRightCalf =  CreateBodySphericalJoint(rightCalf, rightFoot, rCalfPos, NxVec3(1, 0, 0));
      //    jPelvis2 = CreateBodySphericalJoint( neck , spine2, spine2Pos, NxVec3( 0, -1, 0));
      //    NxVec3 leftShoulderPos = NxVec3( torsoPos.x, torsoPos.y + (torsoPos.distance( lUpperArmPos)/2), torsoPos.z);
      //    leftShoulder = CreateBodySphericalJoint( leftUpperArm, torso, leftShoulderPos, NxVec3(1,0,0));
      //    rightShoulder = CreateBodySphericalJoint( rightUpperArm, torso, NxVec3(-0.5,8.5,0), NxVec3(-1,0,0));
      //    NxVec3 spinePos = NxVec3( torsoPos.x, torsoPos.y + ( torsoPos.distance( pelvisPos)/2), torsoPos.z);
      //    spine = CreateBodySphericalJoint( torso, pelvis, spinePos, NxVec3(0,-1,0));
      //    leftHip = CreateBodySphericalJoint(leftThigh,pelvis,NxVec3(0.6,6,0),NxVec3(0,-1,0));
      //    rightHip = CreateBodySphericalJoint(rightThigh,pelvis,NxVec3(-0.6,6,0),NxVec3(0,-1,0));
      //    NxVec3 leftElbowPos = NxVec3( lForeArmPos.x, lForeArmPos.y + ( lForeArmPos.distance( lUpperArmPos)/2), lForeArmPos.z);
      //    leftElbow = CreateRevoluteJoint( leftForeArm, leftUpperArm, leftElbowPos, NxVec3(0,0,-1));
      //    rightElbow = CreateRevoluteJoint( rightForeArm, rightUpperArm,NxVec3(-2,8.5,0),NxVec3(0,0,-1));
      //    NxVec3 leftWristPos = NxVec3( lForeArmPos.x, lForeArmPos.y + ( lForeArmPos.distance( lHandPos)/2), lForeArmPos.z);
      //    leftWrist = CreateRevoluteJoint2( leftHand, leftForeArm, -leftWristPos, leftWristPos, NxVec3(0,1,0), NxVec3(0,1,0));
      //    rightWrist = CreateRevoluteJoint2(rightHand,rightForeArm,NxVec3(0,-0.15,0),NxVec3(0,1.3,0),NxVec3(0,1,0),NxVec3(0,1,0));
      /*
          leftKnee = CreateRevoluteJoint(leftCalf,leftThigh,NxVec3(0.6,3.5,0),NxVec3(1,0,0));
          rightKnee = CreateRevoluteJoint(rightCalf,rightThigh,NxVec3(-0.6,3.5,0),NxVec3(-1,0,0));

          leftAnkle = CreateRevoluteJoint(leftFoot,leftCalf,NxVec3(0.6,1.3,0),NxVec3(1,0,0));
          rightAnkle = CreateRevoluteJoint(rightFoot,rightCalf,NxVec3(-0.6,1.3,0),NxVec3(-1,0,0));
      */
      /*
      NxQuat qRotLeft, qRotRight, qRotAround;
      qRotLeft.fromAngleAxis( 90, NxVec3(0,0,1));
      qRotRight.fromAngleAxis( -90, NxVec3(0,0,1));
      qRotAround.fromAngleAxis( 180, NxVec3(0,0,1));

      NxMat33 mRotLeft, mRotRight, mRotAround;
      mRotLeft.fromQuat(qRotLeft);
      mRotRight.fromQuat(qRotRight);
      mRotAround.fromQuat(qRotAround);

      // Create body parts
      head = CreateSphere( NxVec3(0,8.8,0), 0.5, 10);
      torso = CreateSphere( NxVec3(0,7,0), 0.95, 10);
      pelvis = CreateSphere( NxVec3(0,5.8,0), 0.7, 10);

      leftUpperArm = CreateCapsule(NxVec3(0.5,8.5,0), 1, 0.4, 10);
      leftUpperArm->setGlobalOrientationQuat(qRotRight);
      leftForeArm = CreateCapsule(NxVec3(2,8.5,0), 1, 0.3, 10);
      leftForeArm->setGlobalOrientationQuat(qRotRight);
      leftHand = CreateBox(NxVec3(3.5,8.5,0), NxVec3(0.3,0.3,0.1), 10);
      leftHand->setGlobalOrientationQuat(qRotRight);

      rightUpperArm = CreateCapsule(NxVec3(-0.5,8.5,0), 1, 0.4, 10);
      rightUpperArm->setGlobalOrientationQuat(qRotLeft);
      rightForeArm = CreateCapsule(NxVec3(-2,8.5,0), 1, 0.3, 10);
      rightForeArm->setGlobalOrientationQuat(qRotLeft);
      rightHand = CreateBox(NxVec3(-3.5,8.5,0), NxVec3(0.3,0.3,0.1), 10);
      rightHand->setGlobalOrientationQuat(qRotLeft);

      leftThigh = CreateCapsule(NxVec3(0.6,6,0), 1.5, 0.5, 10);
      leftThigh->setGlobalOrientationQuat(qRotAround);
      leftCalf = CreateCapsule(NxVec3(0.6,3.5,0), 1.5, 0.35, 10);
      leftCalf->setGlobalOrientationQuat(qRotAround);
      leftFoot = CreateBox(NxVec3(0.6,1.5,0.2), NxVec3(0.4,0.2,0.75), 10);
      leftFoot->setGlobalOrientationQuat(qRotAround);

      rightThigh = CreateCapsule(NxVec3(-0.6,6,0), 1.5, 0.5, 10);
      rightThigh->setGlobalOrientationQuat(qRotAround);
      rightCalf = CreateCapsule(NxVec3(-0.6,3.5,0), 1.5, 0.35, 10);
      rightCalf->setGlobalOrientationQuat(qRotAround);
      rightFoot = CreateBox(NxVec3(-0.6,1.5,0.2), NxVec3(0.4,0.2,0.75), 10);
      rightFoot->setGlobalOrientationQuat(qRotAround);

      // Joint body parts together
      neck = CreateBodySphericalJoint(head,torso,NxVec3(0,8.8,0),NxVec3(0,1,0));
      leftShoulder = CreateBodySphericalJoint(leftUpperArm,torso,NxVec3(0.5,8.5,0),NxVec3(1,0,0));
      rightShoulder = CreateBodySphericalJoint(rightUpperArm,torso,NxVec3(-0.5,8.5,0),NxVec3(-1,0,0));
      spine = CreateBodySphericalJoint(torso,pelvis,NxVec3(0,7,0),NxVec3(0,-1,0));
      leftHip = CreateBodySphericalJoint(leftThigh,pelvis,NxVec3(0.6,6,0),NxVec3(0,-1,0));
      rightHip = CreateBodySphericalJoint(rightThigh,pelvis,NxVec3(-0.6,6,0),NxVec3(0,-1,0));

      leftElbow = CreateRevoluteJoint(leftForeArm,leftUpperArm,NxVec3(2,8.5,0),NxVec3(0,0,-1));
      rightElbow = CreateRevoluteJoint(rightForeArm,rightUpperArm,NxVec3(-2,8.5,0),NxVec3(0,0,-1));

      leftWrist = CreateRevoluteJoint2(leftHand,leftForeArm,NxVec3(0,-0.15,0),NxVec3(0,1.3,0),NxVec3(0,1,0),NxVec3(0,1,0));
      rightWrist = CreateRevoluteJoint2(rightHand,rightForeArm,NxVec3(0,-0.15,0),NxVec3(0,1.3,0),NxVec3(0,1,0),NxVec3(0,1,0));

      leftKnee = CreateRevoluteJoint(leftCalf,leftThigh,NxVec3(0.6,3.5,0),NxVec3(1,0,0));
      rightKnee = CreateRevoluteJoint(rightCalf,rightThigh,NxVec3(-0.6,3.5,0),NxVec3(-1,0,0));

      leftAnkle = CreateRevoluteJoint(leftFoot,leftCalf,NxVec3(0.6,1.3,0),NxVec3(1,0,0));
      rightAnkle = CreateRevoluteJoint(rightFoot,rightCalf,NxVec3(-0.6,1.3,0),NxVec3(-1,0,0));
      //*/
    };

    ////////////////////////////////////
    ////////////////////////////////////
    //////JOINTS////////////////////////
    NxFixedJoint* MyRagdoll::CreateFixedJoint(NxActor* a0, NxActor* a1, const NxVec3& globalAnchor, const NxVec3& globalAxis)
    {
      NxFixedJointDesc fixedDesc;
      fixedDesc.actor[0] = a0;
      fixedDesc.actor[1] = a1;
      fixedDesc.setGlobalAnchor(globalAnchor);
      fixedDesc.setGlobalAxis(globalAxis);
      return (NxFixedJoint*)gScene->createJoint(fixedDesc);
    }

    NxRevoluteJoint* MyRagdoll::CreateRevoluteJoint(NxActor* a0, NxActor* a1, const NxVec3& globalAnchor, const NxVec3& globalAxis)
    {
      NxRevoluteJointDesc revDesc;
      revDesc.actor[0] = a0;
      revDesc.actor[1] = a1;
      revDesc.setGlobalAnchor(globalAnchor);
      revDesc.setGlobalAxis(globalAxis);
      return (NxRevoluteJoint*)gScene->createJoint(revDesc);
    }

    NxRevoluteJoint* MyRagdoll::CreateRevoluteJoint2(NxActor* a0, NxActor* a1, const NxVec3& localAnchor0, const NxVec3& localAnchor1, const NxVec3& localAxis0, const NxVec3& localAxis1)
    {
      NxRevoluteJointDesc revDesc;
      revDesc.actor[0] = a0;
      revDesc.actor[1] = a1;
      //revDesc.setGlobalAnchor(globalAnchor);
      //revDesc.setGlobalAxis(globalAxis);
      revDesc.localAnchor[0] = localAnchor0;
      revDesc.localAnchor[1] = localAnchor1;
      revDesc.localAxis[0]  = localAxis0;
      revDesc.localAxis[1]  = localAxis1;
      revDesc.projectionMode = NX_JPM_POINT_MINDIST;
      return (NxRevoluteJoint*)gScene->createJoint(revDesc);
    }


    NxSphericalJoint* MyRagdoll::CreateSphericalJoint(NxActor* a0, NxActor* a1, const NxVec3& globalAnchor, const NxVec3& globalAxis)
    {
      NxSphericalJointDesc sphericalDesc;
      sphericalDesc.actor[0] = a0;
      sphericalDesc.actor[1] = a1;
      sphericalDesc.setGlobalAnchor(globalAnchor);
      sphericalDesc.setGlobalAxis(globalAxis);
      return (NxSphericalJoint*)gScene->createJoint(sphericalDesc);
    }

    NxPrismaticJoint* MyRagdoll::CreatePrismaticJoint(NxActor* a0, NxActor* a1, const NxVec3& globalAnchor, const NxVec3& globalAxis)
    {
      NxPrismaticJointDesc prismaticDesc;
      prismaticDesc.actor[0] = a0;
      prismaticDesc.actor[1] = a1;
      prismaticDesc.setGlobalAnchor(globalAnchor);
      prismaticDesc.setGlobalAxis(globalAxis);
      NxJoint* joint = gScene->createJoint(prismaticDesc);
      //  joint->setLimitPoint(globalAnchor);
      //  joint->addLimitPlane(-globalAxis, globalAnchor + 1.5*globalAxis);
      //  joint->addLimitPlane(globalAxis, globalAnchor - 1.5*globalAxis);
      return (NxPrismaticJoint*)joint;
    }

    NxCylindricalJoint* MyRagdoll::CreateCylindricalJoint(NxActor* a0, NxActor* a1, const NxVec3& globalAnchor, const NxVec3& globalAxis)
    {
      NxCylindricalJointDesc cylDesc;
      cylDesc.actor[0] = a0;
      cylDesc.actor[1] = a1;
      cylDesc.setGlobalAnchor(globalAnchor);
      cylDesc.setGlobalAxis(globalAxis);
      NxJoint* joint = gScene->createJoint(cylDesc);
      return (NxCylindricalJoint*)joint;
    }

    NxPointOnLineJoint* MyRagdoll::CreatePointOnLineJoint(NxActor* a0, NxActor* a1, const NxVec3& globalAnchor, const NxVec3& globalAxis)
    {
      NxPointOnLineJointDesc polDesc;
      polDesc.actor[0] = a0;
      polDesc.actor[1] = a1;
      polDesc.setGlobalAnchor(globalAnchor);
      polDesc.setGlobalAxis(globalAxis);
      polDesc.jointFlags |= NX_JF_COLLISION_ENABLED;
      NxJoint* joint = gScene->createJoint(polDesc);
      return (NxPointOnLineJoint*)joint;
    }

    NxPointInPlaneJoint* MyRagdoll::CreatePointInPlaneJoint(NxActor* a0, NxActor* a1, const NxVec3& globalAnchor, const NxVec3& globalAxis)
    {
      NxPointInPlaneJointDesc pipDesc;
      pipDesc.actor[0] = a0;
      pipDesc.actor[1] = a1;
      pipDesc.setGlobalAnchor(globalAnchor);
      pipDesc.setGlobalAxis(globalAxis);
      pipDesc.jointFlags |= NX_JF_COLLISION_ENABLED;
      NxJoint* joint = gScene->createJoint(pipDesc);
      return (NxPointInPlaneJoint*)joint;
    }

    NxPulleyJoint* MyRagdoll::CreatePulleyJoint(NxActor* a0, NxActor* a1, const NxVec3& pulley0, const NxVec3& pulley1, const NxVec3& globalAxis, NxReal distance, NxReal ratio, const NxMotorDesc& motorDesc)
    {
      NxPulleyJointDesc pulleyDesc;
      pulleyDesc.actor[0] = a0;
      pulleyDesc.actor[1] = a1;
      pulleyDesc.localAnchor[0] = NxVec3(0, 2, 0);
      pulleyDesc.localAnchor[1] = NxVec3(0, 2, 0);
      pulleyDesc.setGlobalAxis(globalAxis);
      pulleyDesc.pulley[0] = pulley0;   // suspension points of two bodies in world space.
      pulleyDesc.pulley[1] = pulley1;   // suspension points of two bodies in world space.
      pulleyDesc.distance = distance;   // the rest length of the rope connecting the two objects.  The distance is computed as ||(pulley0 - anchor0)|| +  ||(pulley1 - anchor1)|| * ratio.
      pulleyDesc.ratio = ratio;     // transmission ratio
      pulleyDesc.flags = NX_PJF_IS_RIGID; // this is a combination of the bits defined by ::NxPulleyJointFlag.
      pulleyDesc.motor = motorDesc;
      pulleyDesc.stiffness = 1;       // how stiff the constraint is, between 0 and 1 (stiffest)
      //  pulleyDesc.projectionMode = NX_JPM_NONE;
      //  pulleyDesc.projectionMode = NX_JPM_POINT_MINDIST;
      pulleyDesc.jointFlags |= NX_JF_COLLISION_ENABLED;
      return (NxPulleyJoint*)gScene->createJoint(pulleyDesc);
    }

    NxDistanceJoint* MyRagdoll::CreateDistanceJoint(NxActor* a0, NxActor* a1, const NxVec3& anchor0, const NxVec3& anchor1, const NxVec3& globalAxis)
    {
      NxDistanceJointDesc distanceDesc;
      distanceDesc.actor[0] = a0;
      distanceDesc.actor[1] = a1;
      distanceDesc.localAnchor[0] = anchor0;
      distanceDesc.localAnchor[1] = anchor1;
      distanceDesc.setGlobalAxis(globalAxis);
      NxVec3 dist = a1->getGlobalPose() * anchor1 - a0->getGlobalPose() * anchor0;
      distanceDesc.maxDistance = dist.magnitude() * 1.5f; // maximum rest length of the rope or rod between the two anchor points
      distanceDesc.minDistance = dist.magnitude() * 0.5f; // minimum rest length of the rope or rod between the two anchor points
      NxSpringDesc spring;
      spring.spring = 100;
      spring.damper = 0.5;
      distanceDesc.spring = spring;  // makes the joint springy. The spring.targetValue field is not used.
      distanceDesc.flags = (NX_DJF_MIN_DISTANCE_ENABLED | NX_DJF_MAX_DISTANCE_ENABLED);  // combination of the bits defined by ::NxDistanceJointFlag
      distanceDesc.flags |= NX_DJF_SPRING_ENABLED;
      //    distanceDesc.projectionMode = NX_JPM_NONE;
      //    distanceDesc.projectionMode = NX_JPM_POINT_MINDIST;
      distanceDesc.jointFlags |= NX_JF_COLLISION_ENABLED;
      return (NxDistanceJoint*)gScene->createJoint(distanceDesc);
    }

    NxSphericalJoint* MyRagdoll::CreateRopeSphericalJoint(NxActor* a0, NxActor* a1, const NxVec3& globalAnchor, const NxVec3& globalAxis)
    {
      NxSphericalJointDesc sphericalDesc;
      sphericalDesc.actor[0] = a0;
      sphericalDesc.actor[1] = a1;
      sphericalDesc.setGlobalAnchor(globalAnchor);
      sphericalDesc.setGlobalAxis(globalAxis);
      sphericalDesc.flags |= NX_SJF_TWIST_LIMIT_ENABLED;
      sphericalDesc.twistLimit.low.value = -(NxReal)0.1 * NxPi;
      sphericalDesc.twistLimit.high.value = (NxReal)0.1 * NxPi;
      sphericalDesc.flags |= NX_SJF_TWIST_SPRING_ENABLED;
      NxSpringDesc ts;
      ts.spring = 500;
      ts.damper = 0.5;
      ts.targetValue = 0;
      sphericalDesc.twistSpring = ts;
      sphericalDesc.flags |= NX_SJF_SWING_LIMIT_ENABLED;
      sphericalDesc.swingLimit.value = (NxReal)0.3 * NxPi;
      sphericalDesc.flags |= NX_SJF_SWING_SPRING_ENABLED;
      NxSpringDesc ss;
      ss.spring = 500;
      ss.damper = 0.5;
      ss.targetValue = 0;
      sphericalDesc.swingSpring = ss;
      return (NxSphericalJoint*)gScene->createJoint(sphericalDesc);
    }

    NxSphericalJoint* MyRagdoll::CreateClothSphericalJoint(NxActor* a0, NxActor* a1, const NxVec3& globalAnchor, const NxVec3& globalAxis)
    {
      NxSphericalJointDesc sphericalDesc;
      sphericalDesc.actor[0] = a0;
      sphericalDesc.actor[1] = a1;
      sphericalDesc.setGlobalAnchor(globalAnchor);
      sphericalDesc.setGlobalAxis(globalAxis);
      sphericalDesc.flags |= NX_SJF_TWIST_LIMIT_ENABLED;
      sphericalDesc.twistLimit.low.value = -(NxReal)0.025 * NxPi;
      sphericalDesc.twistLimit.low.hardness = 0.5;
      sphericalDesc.twistLimit.low.restitution = 0.5;
      sphericalDesc.twistLimit.high.value = (NxReal)0.025 * NxPi;
      sphericalDesc.twistLimit.high.hardness = 0.5;
      sphericalDesc.twistLimit.high.restitution = 0.5;
      sphericalDesc.flags |= NX_SJF_SWING_LIMIT_ENABLED;
      sphericalDesc.swingLimit.value = (NxReal)0.25 * NxPi;
      sphericalDesc.swingLimit.hardness = 0.5;
      sphericalDesc.swingLimit.restitution = 0.5;
      sphericalDesc.flags |= NX_SJF_TWIST_SPRING_ENABLED;
      sphericalDesc.twistSpring.spring = 0.5;
      sphericalDesc.twistSpring.damper = 1;
      sphericalDesc.flags |= NX_SJF_SWING_SPRING_ENABLED;
      sphericalDesc.swingSpring.spring = 0.5;
      sphericalDesc.swingSpring.damper = 1;
      //  sphericalDesc.flags |= NX_SJF_JOINT_SPRING_ENABLED;
      //  sphericalDesc.jointSpring.spring = 0.5;
      //  sphericalDesc.jointSpring.damper = 1;
      sphericalDesc.projectionDistance = (NxReal)0.15;
      sphericalDesc.projectionMode = NX_JPM_POINT_MINDIST;
      return (NxSphericalJoint*)gScene->createJoint(sphericalDesc);
    }

    NxSphericalJoint* MyRagdoll::CreateBodySphericalJoint(NxActor* a0, NxActor* a1, const NxVec3& globalAnchor, const NxVec3& globalAxis)
    {
      NxSphericalJointDesc sphericalDesc;
      sphericalDesc.actor[0] = a0;
      sphericalDesc.actor[1] = a1;
      sphericalDesc.setGlobalAnchor(globalAnchor);
      sphericalDesc.setGlobalAxis(globalAxis);
      sphericalDesc.flags |= NX_SJF_TWIST_LIMIT_ENABLED;
      sphericalDesc.twistLimit.low.value = -(NxReal)0.025 * NxPi;
      sphericalDesc.twistLimit.low.hardness = 0.5;
      sphericalDesc.twistLimit.low.restitution = 0.5;
      sphericalDesc.twistLimit.high.value = (NxReal)0.025 * NxPi;
      sphericalDesc.twistLimit.high.hardness = 0.5;
      sphericalDesc.twistLimit.high.restitution = 0.5;
      sphericalDesc.flags |= NX_SJF_SWING_LIMIT_ENABLED;
      sphericalDesc.swingLimit.value = (NxReal)0.25 * NxPi;
      sphericalDesc.swingLimit.hardness = 0.5;
      sphericalDesc.swingLimit.restitution = 0.5;
      sphericalDesc.flags |= NX_SJF_TWIST_SPRING_ENABLED;
      sphericalDesc.twistSpring.spring = 0.5;
      sphericalDesc.twistSpring.damper = 1;
      sphericalDesc.flags |= NX_SJF_SWING_SPRING_ENABLED;
      sphericalDesc.swingSpring.spring = 0.5;
      sphericalDesc.swingSpring.damper = 1;
      //  sphericalDesc.flags |= NX_SJF_JOINT_SPRING_ENABLED;
      //  sphericalDesc.jointSpring.spring = 0.5;
      //  sphericalDesc.jointSpring.damper = 1;
      sphericalDesc.projectionDistance = (NxReal)0.15;
      sphericalDesc.projectionMode = NX_JPM_POINT_MINDIST;
      return (NxSphericalJoint*)gScene->createJoint(sphericalDesc);
    }

    NxRevoluteJoint* MyRagdoll::CreateWheelJoint(NxActor* a0, NxActor* a1, const NxVec3& globalAnchor, const NxVec3& globalAxis)
    {
      NxRevoluteJointDesc revDesc;
      revDesc.actor[0] = a0;
      revDesc.actor[1] = a1;
      revDesc.setGlobalAnchor(globalAnchor);
      revDesc.setGlobalAxis(globalAxis);
      return (NxRevoluteJoint*)gScene->createJoint(revDesc);
    }

    NxRevoluteJoint* MyRagdoll::CreateStepJoint(NxActor* a0, NxActor* a1, const NxVec3& globalAnchor, const NxVec3& globalAxis)
    {
      NxRevoluteJointDesc revDesc;
      revDesc.actor[0] = a0;
      revDesc.actor[1] = a1;
      revDesc.setGlobalAnchor(globalAnchor);
      revDesc.setGlobalAxis(globalAxis);
      return (NxRevoluteJoint*)gScene->createJoint(revDesc);
    }

    NxRevoluteJoint* MyRagdoll::CreateChassisJoint(NxActor* a0, NxActor* a1, const NxVec3& globalAnchor, const NxVec3& globalAxis)
    {
      NxRevoluteJointDesc revDesc;
      revDesc.actor[0] = a0;
      revDesc.actor[1] = a1;
      revDesc.setGlobalAnchor(globalAnchor);
      revDesc.setGlobalAxis(globalAxis);
      revDesc.flags |= NX_RJF_LIMIT_ENABLED;
      NxJointLimitPairDesc limitDesc;
      limitDesc.high.value = (NxReal)0.01 * NxPi;
      limitDesc.low.value = -(NxReal)0.01 * NxPi;;
      revDesc.limit = limitDesc;
      revDesc.flags |= NX_RJF_SPRING_ENABLED;
      NxSpringDesc springDesc;
      springDesc.targetValue = 0;
      springDesc.spring = 5000;
      //  motorDesc.maxForce = 100;
      //  motorDesc.freeSpin = 0;
      //
      //  revDesc.motor = motorDesc;
      return (NxRevoluteJoint*)gScene->createJoint(revDesc);
    }

    NxFixedJoint* MyRagdoll::CreateCannonJoint(NxActor* a0, NxActor* a1, const NxVec3& globalAnchor, const NxVec3& globalAxis)
    {
      NxFixedJointDesc fixDesc;
      fixDesc.actor[0] = a0;
      fixDesc.actor[1] = a1;
      fixDesc.setGlobalAnchor(globalAnchor);
      fixDesc.setGlobalAxis(globalAxis);
      return (NxFixedJoint*)gScene->createJoint(fixDesc);
    }

    NxSphericalJoint* MyRagdoll::CreateBladeLink(NxActor* a0, NxActor* a1, const NxVec3& globalAnchor, const NxVec3& globalAxis)
    {
      NxSphericalJointDesc sphericalDesc;
      sphericalDesc.actor[0] = a0;
      sphericalDesc.actor[1] = a1;
      sphericalDesc.setGlobalAnchor(globalAnchor);
      sphericalDesc.setGlobalAxis(globalAxis);
      sphericalDesc.flags |= NX_SJF_SWING_LIMIT_ENABLED;
      sphericalDesc.swingLimit.value = (NxReal)0.05 * NxPi;
      sphericalDesc.swingLimit.restitution = 0.75;
      sphericalDesc.swingLimit.hardness = 0.5;
      sphericalDesc.flags |= NX_SJF_SWING_SPRING_ENABLED;
      sphericalDesc.swingSpring.spring = 0.75;
      sphericalDesc.swingSpring.damper = 1;
      sphericalDesc.flags |= NX_SJF_TWIST_LIMIT_ENABLED;
      sphericalDesc.twistLimit.low.value = -(NxReal)0.05 * NxPi;
      sphericalDesc.twistLimit.low.restitution = 0.75;
      sphericalDesc.twistLimit.low.hardness = 0.5;
      sphericalDesc.twistLimit.high.value = (NxReal)0.05 * NxPi;
      sphericalDesc.twistLimit.high.restitution = 0.75;
      sphericalDesc.twistLimit.high.hardness = 0.5;
      sphericalDesc.flags |= NX_SJF_TWIST_SPRING_ENABLED;
      sphericalDesc.twistSpring.spring = 0.75;
      sphericalDesc.twistSpring.damper = 1;
      return (NxSphericalJoint*)gScene->createJoint(sphericalDesc);
    }

    //////////////////////////////////////////////
    //////////////////////////////////////////////
    ///////////////////ACtors

    NxActor* MyRagdoll::CreateGroundPlane()
    {
      assert(0 != gScene);
      // Create a plane with default descriptor
      NxPlaneShapeDesc planeDesc;
      NxActorDesc actorDesc;
      actorDesc.shapes.pushBack(&planeDesc);
      return gScene->createActor(actorDesc);
    }

    NxActor* MyRagdoll::CreateBox(const NxVec3& pos, const NxVec3& boxDim, const NxReal density)
    {
      assert(0 != gScene);
      NxBoxShapeDesc boxDesc;
      boxDesc.dimensions.set(boxDim.x, boxDim.y, boxDim.z);
      boxDesc.localPose.t = NxVec3(0, boxDim.y, 0);
      NxActorDesc actorDesc;
      actorDesc.shapes.pushBack(&boxDesc);
      actorDesc.globalPose.t = pos;
      NxBodyDesc bodyDesc;

      if(density)
      {
        actorDesc.body = &bodyDesc;
        actorDesc.density = density;
      }
      else
      {
        actorDesc.body = 0;
      }

      return gScene->createActor(actorDesc);
    }

    NxActor* MyRagdoll::CreateSphere(const NxVec3& pos, const NxReal radius, const NxReal density)
    {
      assert(0 != gScene);
      NxSphereShapeDesc sphereDesc;
      sphereDesc.radius = radius;
      sphereDesc.localPose.t = NxVec3(0, radius, 0);
      NxActorDesc actorDesc;
      actorDesc.shapes.pushBack(&sphereDesc);
      actorDesc.globalPose.t = pos;
      NxBodyDesc bodyDesc;

      if(density)
      {
        actorDesc.body = &bodyDesc;
        actorDesc.density = density;
      }
      else
      {
        actorDesc.body = 0;
      }

      return gScene->createActor(actorDesc);
    }

    NxActor* MyRagdoll::CreateCapsule(const NxVec3& pos, const NxReal height, const NxReal radius, const NxReal density)
    {
      assert(0 != gScene);
      NxCapsuleShapeDesc capsuleDesc;
      capsuleDesc.height = height;
      capsuleDesc.radius = radius;
      capsuleDesc.localPose.t = NxVec3(0, radius + 0.5f * height, 0);
      NxActorDesc actorDesc;
      actorDesc.shapes.pushBack(&capsuleDesc);
      actorDesc.globalPose.t = pos;
      NxBodyDesc bodyDesc;

      if(density)
      {
        actorDesc.body = &bodyDesc;
        actorDesc.density = density;
      }
      else
      {
        actorDesc.body = 0;
      }

      return gScene->createActor(actorDesc);
    }

    NxActor* MyRagdoll::CreateHalfPyramid(const NxVec3& pos, const NxVec3& boxDim, const NxReal density)
    {
      NxActorDesc actorDesc;
      NxBodyDesc bodyDesc;
      // Half-Pyramid
      NxVec3 verts[8] = { NxVec3(boxDim.x, -boxDim.y, -boxDim.z), NxVec3(-boxDim.x, -boxDim.y, -boxDim.z), NxVec3(-boxDim.x, -boxDim.y, boxDim.z), NxVec3(boxDim.x, -boxDim.y, boxDim.z),
                          NxVec3(boxDim.x * (NxReal)0.5, boxDim.y, -boxDim.z * (NxReal)0.5), NxVec3(-boxDim.x * (NxReal)0.5, boxDim.y, -boxDim.z * (NxReal)0.5), NxVec3(-boxDim.x * (NxReal)0.5, boxDim.y, boxDim.z * (NxReal)0.5), NxVec3(boxDim.x * (NxReal)0.5, boxDim.y, boxDim.z * (NxReal)0.5)
                        };
      // Create descriptor for convex mesh
      NxConvexMeshDesc convexDesc;
      convexDesc.numVertices      = 8;
      convexDesc.pointStrideBytes   = sizeof(NxVec3);
      convexDesc.points       = verts;
      convexDesc.flags        = NX_CF_COMPUTE_CONVEX;
      NxConvexShapeDesc convexShapeDesc;
      convexShapeDesc.localPose.t   = NxVec3(0, boxDim.y, 0);
      NxInitCooking();

      if(0)
      {
        // Cooking from file
#ifdef WIN32
        bool status = NxCookConvexMesh(convexDesc, UserStream("c:\\tmp.bin", false));
        convexShapeDesc.meshData = gPhysicsSDK->createConvexMesh(UserStream("c:\\tmp.bin", true));
#endif
      }
      else
      {
        // Cooking from memory
        MemoryWriteBuffer buf;
        bool status = NxCookConvexMesh(convexDesc, buf);
        convexShapeDesc.meshData = gPhysicsSDK->createConvexMesh(MemoryReadBuffer(buf.data));
      }

      if(convexShapeDesc.meshData)
      {
        NxActorDesc actorDesc;
        actorDesc.shapes.pushBack(&convexShapeDesc);

        if(density)
        {
          actorDesc.body = &bodyDesc;
          actorDesc.density = density;
        }
        else
        {
          actorDesc.body = NULL;
        }

        actorDesc.globalPose.t  = pos;
        NxActor* actor = gScene->createActor(actorDesc);
        return actor;
        //      gPhysicsSDK->releaseConvexMesh(*convexShapeDesc.meshData);
      }

      return NULL;
    }

    NxActor* MyRagdoll::CreatePyramid(const NxVec3& pos, const NxVec3& boxDim, const NxReal density)
    {
      NxActorDesc actorDesc;
      NxBodyDesc bodyDesc;
      // Pyramid
      NxVec3 verts[5] = { NxVec3(boxDim.x, -boxDim.y, -boxDim.z), NxVec3(-boxDim.x, -boxDim.y, -boxDim.z), NxVec3(-boxDim.x, -boxDim.y, boxDim.z), NxVec3(boxDim.x, -boxDim.y, boxDim.z),
                          NxVec3(0, boxDim.y, 0)
                        };
      // Create descriptor for convex mesh
      NxConvexMeshDesc* convexDesc = new NxConvexMeshDesc();
      convexDesc->numVertices     = 5;
      convexDesc->pointStrideBytes  = sizeof(NxVec3);
      convexDesc->points        = verts;
      convexDesc->flags       = NX_CF_COMPUTE_CONVEX;
      NxConvexShapeDesc convexShapeDesc;
      convexShapeDesc.localPose.t   = NxVec3(0, boxDim.y, 0);
      convexShapeDesc.userData = convexDesc;
      NxInitCooking();

      if(0)
      {
        // Cooking from file
#ifdef WIN32
        bool status = NxCookConvexMesh(*convexDesc, UserStream("c:\\tmp.bin", false));
        convexShapeDesc.meshData = gPhysicsSDK->createConvexMesh(UserStream("c:\\tmp.bin", true));
#endif
      }
      else
      {
        // Cooking from memory
        MemoryWriteBuffer buf;
        bool status = NxCookConvexMesh(*convexDesc, buf);
        convexShapeDesc.meshData = gPhysicsSDK->createConvexMesh(MemoryReadBuffer(buf.data));
      }

      if(convexShapeDesc.meshData)
      {
        NxActorDesc actorDesc;
        actorDesc.shapes.pushBack(&convexShapeDesc);

        if(density)
        {
          actorDesc.body = &bodyDesc;
          actorDesc.density = density;
        }
        else
        {
          actorDesc.body = NULL;
        }

        actorDesc.globalPose.t  = pos;
        NxActor* actor = gScene->createActor(actorDesc);
        return actor;
        //      gPhysicsSDK->releaseTriangleMesh(*convexShapeDesc.meshData);
      }

      return NULL;
    }

    NxActor* MyRagdoll::CreateDownWedge(const NxVec3& pos, const NxVec3& boxDim, const NxReal density)
    {
      NxActorDesc actorDesc;
      NxBodyDesc bodyDesc;
      // Down wedge
      NxVec3 verts[6] = { NxVec3(boxDim.x, boxDim.y, -boxDim.z), NxVec3(boxDim.x, boxDim.y, boxDim.z), NxVec3(boxDim.x, -boxDim.y, 0),
                          NxVec3(-boxDim.x, boxDim.y, -boxDim.z), NxVec3(-boxDim.x, boxDim.y, boxDim.z), NxVec3(-boxDim.x, -boxDim.y, 0)
                        };
      // Create descriptor for convex mesh
      NxConvexMeshDesc convexDesc;
      convexDesc.numVertices      = 6;
      convexDesc.pointStrideBytes   = sizeof(NxVec3);
      convexDesc.points       = verts;
      convexDesc.flags        = NX_CF_COMPUTE_CONVEX;
      NxConvexShapeDesc convexShapeDesc;
      convexShapeDesc.localPose.t   = NxVec3(0, boxDim.y, 0);
      NxInitCooking();

      if(0)
      {
        // Cooking from file
#ifdef WIN32
        bool status = NxCookConvexMesh(convexDesc, UserStream("c:\\tmp.bin", false));
        convexShapeDesc.meshData = gPhysicsSDK->createConvexMesh(UserStream("c:\\tmp.bin", true));
#endif
      }
      else
      {
        // Cooking from memory
        MemoryWriteBuffer buf;
        bool status = NxCookConvexMesh(convexDesc, buf);
        convexShapeDesc.meshData = gPhysicsSDK->createConvexMesh(MemoryReadBuffer(buf.data));
      }

      if(convexShapeDesc.meshData)
      {
        NxActorDesc actorDesc;
        actorDesc.shapes.pushBack(&convexShapeDesc);

        if(density)
        {
          actorDesc.body = &bodyDesc;
          actorDesc.density = density;
        }
        else
        {
          actorDesc.body = NULL;
        }

        actorDesc.globalPose.t  = pos;
        NxActor* actor = gScene->createActor(actorDesc);
        return actor;
        //      gPhysicsSDK->releaseConvexMesh(*convexShapeDesc.meshData);
      }

      return NULL;
    }

    NxActor* MyRagdoll::CreateFlatSurface(const NxVec3& pos, const NxU32 length, const NxU32 width, const NxReal stride)
    {
      NxVec3* fsVerts = NULL;
      NxU32* fsFaces = NULL;
      NxVec3* fsNormals = NULL;
      // Initialize flat surface verts
      NxU32 nbVerts = (length + 1) * (width + 1);
      fsVerts = new NxVec3[nbVerts];

      // Build flat surface
      for(NxU32 i = 0; i < width + 1; ++i)
      {
        for(NxU32 j = 0; j < length + 1; ++j)
        {
          fsVerts[i + j * (width + 1)] = NxVec3(stride * NxReal(i), 0, stride * NxReal(j)) - NxVec3(0.5f * stride * NxReal(width), 0, 0.5f * stride * NxReal(length));
        }
      }

      // Initialize flat surface faces
      NxU32 nbFaces = length * width * 2;
      fsFaces = new NxU32[nbFaces * 3];
      NxU32 k = 0;

      for(NxU32 i = 0; i < width; ++i)
      {
        for(NxU32 j = 0; j < length; ++j)
        {
          // Create first triangle
          fsFaces[k] = i + j * (width + 1);
          fsFaces[k + 1] = i + (j + 1) * (width + 1);
          fsFaces[k + 2] = i + 1 + (j + 1) * (width + 1);
          k += 3;
          // Create second triangle
          fsFaces[k] = i + j * (width + 1);
          fsFaces[k + 1] = i + 1 + (j + 1) * (width + 1);
          fsFaces[k + 2] = i + 1 + j * (width + 1);
          k += 3;
        }
      }

      // allocate flat surface materials -- one for each face
      //  fsMaterials = new NxMaterialIndex[nbFaces];
      // Build vertex normals
      fsNormals = new NxVec3[nbFaces];
      //  NxBuildSmoothNormals(nbFaces, nbVerts, fsVerts, fsFaces, NULL, fsNormals, true);
      NxTriangleMeshDesc fsDesc;
      fsDesc.numVertices        = nbVerts;
      fsDesc.numTriangles       = nbFaces;
      fsDesc.pointStrideBytes     = sizeof(NxVec3);
      fsDesc.triangleStrideBytes    = 3 * sizeof(NxU32);
      fsDesc.points         = fsVerts;
      fsDesc.triangles        = fsFaces;
      fsDesc.flags          = NX_MF_HARDWARE_MESH;
      // Add the mesh material data
      //  fsDesc.materialIndexStride      = sizeof(NxMaterialIndex);
      //  fsDesc.materialIndices        = fsMaterials;
      //  fsDesc.heightFieldVerticalAxis    = NX_Y;
      //  fsDesc.heightFieldVerticalExtent  = 0;
      NxTriangleMeshShapeDesc fsShapeDesc;
      NxInitCooking();

      if(0)
      {
#ifdef WIN32
        // Cooking from file
        bool status = NxCookTriangleMesh(fsDesc, UserStream("c:\\tmp.bin", false));
        fsShapeDesc.meshData = gPhysicsSDK->createTriangleMesh(UserStream("c:\\tmp.bin", true));
#endif
      }
      else
      {
        // Cooking from memory
        MemoryWriteBuffer buf;
        bool status = NxCookTriangleMesh(fsDesc, buf);
        fsShapeDesc.meshData = gPhysicsSDK->createTriangleMesh(MemoryReadBuffer(buf.data));
      }

      if(fsShapeDesc.meshData)
      {
        NxActorDesc actorDesc;
        actorDesc.shapes.pushBack(&fsShapeDesc);
        actorDesc.globalPose.t = pos;
        NxActor* actor = gScene->createActor(actorDesc);
        return actor;
        //      gPhysicsSDK->releaseTriangleMesh(*fsShapeDesc.meshData);
      }

      return NULL;
    }

    NxActor* MyRagdoll::CreateFlatHeightfield(const NxVec3& pos, const NxU32 length, const NxU32 width, const NxReal stride)
    {
      NxVec3* hfVerts = NULL;
      NxU32* hfFaces = NULL;
      NxVec3* hfNormals = NULL;
      // Initialize heightfield verts
      NxU32 nbVerts = (length + 1) * (width + 1);
      hfVerts = new NxVec3[nbVerts];

      // Build flat heightfield
      for(NxU32 i = 0; i < width + 1; ++i)
      {
        for(NxU32 j = 0; j < length + 1; ++j)
        {
          hfVerts[i + j * (width + 1)] = NxVec3(stride * NxReal(i), 0.5, stride * NxReal(j));
        }
      }

      // Initialize heightfield faces
      NxU32 nbFaces = length * width * 2;
      hfFaces = new NxU32[nbFaces * 3];
      NxU32 k = 0;

      for(NxU32 i = 0; i < width; ++i)
      {
        for(NxU32 j = 0; j < length; ++j)
        {
          // Create first triangle
          hfFaces[k] = i + j * (width + 1);
          hfFaces[k + 1] = i + (j + 1) * (width + 1);
          hfFaces[k + 2] = i + 1 + (j + 1) * (width + 1);
          k += 3;
          // Create second triangle
          hfFaces[k] = i + j * (width + 1);
          hfFaces[k + 1] = i + 1 + (j + 1) * (width + 1);
          hfFaces[k + 2] = i + 1 + j * (width + 1);
          k += 3;
        }
      }

      // allocate heightfield materials -- one for each face
      //  hfMaterials = new NxMaterialIndex[nbFaces];
      // Build vertex normals
      hfNormals = new NxVec3[nbFaces];
      //  NxBuildSmoothNormals(nbFaces, nbVerts, hfVerts, hfFaces, NULL, hfNormals, true);
      NxTriangleMeshDesc hfDesc;
      hfDesc.numVertices        = nbVerts;
      hfDesc.numTriangles       = nbFaces;
      hfDesc.pointStrideBytes     = sizeof(NxVec3);
      hfDesc.triangleStrideBytes    = 3 * sizeof(NxU32);
      hfDesc.points         = hfVerts;
      hfDesc.triangles        = hfFaces;
      hfDesc.flags          = 0;
      // Add the mesh material data
      //  hfDesc.materialIndexStride      = sizeof(NxMaterialIndex);
      //  hfDesc.materialIndices        = hfMaterials;
      hfDesc.heightFieldVerticalAxis    = NX_Y;
      hfDesc.heightFieldVerticalExtent  = 0;
      NxTriangleMeshShapeDesc hfShapeDesc;
      NxInitCooking();

      if(0)
      {
#ifdef WIN32
        // Cooking from file
        bool status = NxCookTriangleMesh(hfDesc, UserStream("c:\\tmp.bin", false));
        hfShapeDesc.meshData = gPhysicsSDK->createTriangleMesh(UserStream("c:\\tmp.bin", true));
#endif
      }
      else
      {
        // Cooking from memory
        MemoryWriteBuffer buf;
        bool status = NxCookTriangleMesh(hfDesc, buf);
        hfShapeDesc.meshData = gPhysicsSDK->createTriangleMesh(MemoryReadBuffer(buf.data));
      }

      if(hfShapeDesc.meshData)
      {
        NxActorDesc actorDesc;
        actorDesc.shapes.pushBack(&hfShapeDesc);
        actorDesc.globalPose.t = pos;
        NxActor* actor = gScene->createActor(actorDesc);
        return actor;
        //      gPhysicsSDK->releaseTriangleMesh(*hfShapeDesc.meshData);
      }

      return NULL;
    }

    NxActor* MyRagdoll::CreateConvexObjectComputeHull(const NxVec3& pos, const NxVec3& boxDim, const NxReal density)
    {
      NxActorDesc actorDesc;
      NxBodyDesc bodyDesc;
      // Compute hull
      NxVec3 verts[8] =
      {
        NxVec3(-boxDim.x, -boxDim.y, -boxDim.z),
        NxVec3(boxDim.x, -boxDim.y, -boxDim.z),
        NxVec3(-boxDim.x, boxDim.y, -boxDim.z),
        NxVec3(boxDim.x, boxDim.y, -boxDim.z),
        NxVec3(-boxDim.x, -boxDim.y, boxDim.z),
        NxVec3(boxDim.x, -boxDim.y, boxDim.z),
        NxVec3(-boxDim.x, boxDim.y, boxDim.z),
        NxVec3(boxDim.x, boxDim.y, boxDim.z)
      };
      // Create descriptor for convex mesh
      NxConvexMeshDesc convexDesc;
      convexDesc.numVertices      = 8;
      convexDesc.pointStrideBytes   = sizeof(NxVec3);
      convexDesc.points       = verts;
      convexDesc.flags        = NX_CF_COMPUTE_CONVEX;
      NxConvexShapeDesc convexShapeDesc;
      convexShapeDesc.localPose.t   = NxVec3(0, boxDim.y, 0);
      NxInitCooking();

      if(0)
      {
        // Cooking from file
#ifdef WIN32
        bool status = NxCookConvexMesh(convexDesc, UserStream("c:\\tmp.bin", false));
        convexShapeDesc.meshData = gPhysicsSDK->createConvexMesh(UserStream("c:\\tmp.bin", true));
#endif
      }
      else
      {
        // Cooking from memory
        MemoryWriteBuffer buf;
        bool status = NxCookConvexMesh(convexDesc, buf);
        convexShapeDesc.meshData = gPhysicsSDK->createConvexMesh(MemoryReadBuffer(buf.data));
      }

      if(convexShapeDesc.meshData)
      {
        NxActorDesc actorDesc;
        actorDesc.shapes.pushBack(&convexShapeDesc);

        if(density)
        {
          actorDesc.body = &bodyDesc;
          actorDesc.density = density;
        }
        else
        {
          actorDesc.body = NULL;
        }

        actorDesc.globalPose.t  = pos;
        NxActor* actor = gScene->createActor(actorDesc);
        return actor;
        //      gPhysicsSDK->releaseConvexMesh(*convexShapeDesc.meshData);
      }

      return NULL;
    }

    NxActor* MyRagdoll::CreateConvexObjectSupplyHull(const NxVec3& pos, const NxVec3& boxDim, const NxReal density)
    {
      NxActorDesc actorDesc;
      NxBodyDesc bodyDesc;
      // Supply hull
      NxVec3 verts[8] =
      {
        NxVec3(-boxDim.x, -boxDim.y, -boxDim.z),
        NxVec3(boxDim.x, -boxDim.y, -boxDim.z),
        NxVec3(-boxDim.x, boxDim.y, -boxDim.z),
        NxVec3(boxDim.x, boxDim.y, -boxDim.z),
        NxVec3(-boxDim.x, -boxDim.y, boxDim.z),
        NxVec3(boxDim.x, -boxDim.y, boxDim.z),
        NxVec3(-boxDim.x, boxDim.y, boxDim.z),
        NxVec3(boxDim.x, boxDim.y, boxDim.z)
      };
      NxU32 indices[12 * 3] =
      {
        1, 2, 3,
        0, 2, 1,
        5, 7, 6,
        4, 5, 6,
        5, 4, 1,
        1, 4, 0,
        1, 3, 5,
        3, 7, 5,
        3, 2, 7,
        2, 6, 7,
        2, 0, 6,
        4, 6, 0
      };
      // Create descriptor for triangle mesh
      NxConvexMeshDesc convexDesc;
      convexDesc.numVertices      = 8;
      convexDesc.pointStrideBytes   = sizeof(NxVec3);
      convexDesc.points       = verts;
      convexDesc.numTriangles     = 12;
      convexDesc.triangles      = indices;
      convexDesc.triangleStrideBytes  = 3 * sizeof(NxU32);
      convexDesc.flags        = 0;
      NxConvexShapeDesc convexShapeDesc;
      convexShapeDesc.localPose.t   = NxVec3(0, boxDim.y, 0);
      NxInitCooking();

      if(0)
      {
        // Cooking from file
#ifdef WIN32
        bool status = NxCookConvexMesh(convexDesc, UserStream("c:\\tmp.bin", false));
        convexShapeDesc.meshData = gPhysicsSDK->createConvexMesh(UserStream("c:\\tmp.bin", true));
#endif
      }
      else
      {
        // Cooking from memory
        MemoryWriteBuffer buf;
        bool status = NxCookConvexMesh(convexDesc, buf);
        convexShapeDesc.meshData = gPhysicsSDK->createConvexMesh(MemoryReadBuffer(buf.data));
      }

      if(convexShapeDesc.meshData)
      {
        NxActorDesc actorDesc;
        actorDesc.shapes.pushBack(&convexShapeDesc);

        if(density)
        {
          actorDesc.body = &bodyDesc;
          actorDesc.density = density;
        }
        else
        {
          actorDesc.body = NULL;
        }

        actorDesc.globalPose.t  = pos;
        NxActor* actor = gScene->createActor(actorDesc);
        return actor;
        //      gPhysicsSDK->releaseConvexMesh(*convexShapeDesc.meshData);
      }

      return NULL;
    }

    NxActor* MyRagdoll::CreateConcaveObject(const NxVec3& pos, const NxVec3& boxDim, const NxReal density)
    {
      NxActorDesc actorDesc;
      NxBodyDesc bodyDesc;
      // Supply hull
      NxVec3 verts[8] =
      {
        NxVec3(-boxDim.x, -boxDim.y, -boxDim.z),
        NxVec3(boxDim.x, -boxDim.y, -boxDim.z),
        NxVec3(-boxDim.x, boxDim.y, -boxDim.z),
        NxVec3(boxDim.x, boxDim.y, -boxDim.z),
        NxVec3(-boxDim.x, -boxDim.y, boxDim.z),
        NxVec3(boxDim.x, -boxDim.y, boxDim.z),
        NxVec3(-boxDim.x, boxDim.y, boxDim.z),
        NxVec3(boxDim.x, boxDim.y, boxDim.z)
      };
      NxU32 indices[12 * 3] =
      {
        1, 2, 3,
        0, 2, 1,
        5, 7, 6,
        4, 5, 6,
        5, 4, 1,
        1, 4, 0,
        1, 3, 5,
        3, 7, 5,
        3, 2, 7,
        2, 6, 7,
        2, 0, 6,
        4, 6, 0
      };
      // Create descriptor for convex mesh
      NxConvexMeshDesc convexDesc;
      convexDesc.numVertices      = 8;
      convexDesc.pointStrideBytes = sizeof(NxVec3);
      convexDesc.points       = verts;
      convexDesc.numTriangles       = 12;
      convexDesc.triangles      = indices;
      convexDesc.triangleStrideBytes  = 3 * sizeof(NxU32);
      convexDesc.flags        = 0;
      // Create descriptor for convex mesh shape #1
      NxConvexShapeDesc convexShapeDesc1;
      convexShapeDesc1.localPose.t    = NxVec3(0, boxDim.y, 0);
      // Create descriptor for convex mesh shape #2
      NxConvexShapeDesc convexShapeDesc2;
      convexShapeDesc2.localPose.t    = NxVec3(0, boxDim.y, 2 * boxDim.z);
      // Create descriptor for convex mesh shape #3
      NxConvexShapeDesc convexShapeDesc3;
      convexShapeDesc3.localPose.t    = NxVec3(2 * boxDim.x, boxDim.y, 2 * boxDim.z);
      NxInitCooking();

      if(0)
      {
        // Cooking from file
#ifdef WIN32
        bool status = NxCookConvexMesh(convexDesc, UserStream("c:\\tmp.bin", false));
        convexShapeDesc1.meshData = gPhysicsSDK->createConvexMesh(UserStream("c:\\tmp.bin", true));
        convexShapeDesc2.meshData = gPhysicsSDK->createConvexMesh(UserStream("c:\\tmp.bin", true));
        convexShapeDesc3.meshData = gPhysicsSDK->createConvexMesh(UserStream("c:\\tmp.bin", true));
#endif
      }
      else
      {
        // Cooking from memory
        MemoryWriteBuffer buf;
        bool status = NxCookConvexMesh(convexDesc, buf);
        convexShapeDesc1.meshData = gPhysicsSDK->createConvexMesh(MemoryReadBuffer(buf.data));
        convexShapeDesc2.meshData = gPhysicsSDK->createConvexMesh(MemoryReadBuffer(buf.data));
        convexShapeDesc3.meshData = gPhysicsSDK->createConvexMesh(MemoryReadBuffer(buf.data));
      }

      if(convexShapeDesc1.meshData && convexShapeDesc2.meshData && convexShapeDesc3.meshData)
      {
        NxActorDesc actorDesc;
        actorDesc.shapes.pushBack(&convexShapeDesc1);
        actorDesc.shapes.pushBack(&convexShapeDesc2);
        actorDesc.shapes.pushBack(&convexShapeDesc3);

        if(density)
        {
          actorDesc.body = &bodyDesc;
          actorDesc.density = density;
        }
        else
        {
          actorDesc.body = NULL;
        }

        actorDesc.globalPose.t  = pos;
        NxActor* actor = gScene->createActor(actorDesc);
        return actor;
        //      gPhysicsSDK->releaseConvexMesh(*convexShapeDesc.meshData);
      }

      return NULL;
    }

    NxActor* MyRagdoll::CreateBunny(const NxVec3& pos, NxTriangleMesh* triangleMesh, const NxReal density)
    {
      NxBodyDesc bodyDesc;
      //create triangle mesh instance
      NxTriangleMeshShapeDesc bunnyShapeDesc;
      bunnyShapeDesc.meshData = triangleMesh;
      bunnyShapeDesc.localPose.t = NxVec3(0, 1, 0);

      if(bunnyShapeDesc.meshData)
      {
        NxActorDesc actorDesc;
        actorDesc.shapes.pushBack(&bunnyShapeDesc);

        if(density)
        {
          actorDesc.body = &bodyDesc;
          actorDesc.density = density;
        }
        else
        {
          actorDesc.body = NULL;
        }

        actorDesc.globalPose.t  = pos;
        NxActor* actor = gScene->createActor(actorDesc);
        return actor;
      }

      return NULL;
    }

    NxActor** MyRagdoll::CreateStack(const NxVec3& pos, const NxVec3& stackDim, const NxVec3& boxDim, NxReal density)
    {
      // so ugly codes!!!  should be improved
      NxActor** stack = new NxActor*[(int)(8 * stackDim.x * stackDim.y * stackDim.z)];
      int count = 0;
      NxVec3 offset = NxVec3(boxDim.x, 0, boxDim.z) + pos;

      for(int i = -(int)stackDim.x; i < (int)stackDim.x; ++i)
      {
        for(int j = 0; j < (int)stackDim.y; ++j)
        {
          for(int k = -(int)stackDim.z; k < (int)stackDim.z; ++k)
          {
            NxVec3 boxPos = NxVec3(i * boxDim.x * 2, j * boxDim.y * 2, k * boxDim.z * 2);
            stack[++count] = CreateBox(boxPos + offset, boxDim, density);
          }
        }
      }

      return stack;
    }

    NxActor** MyRagdoll::CreateTower(const NxVec3& pos, const NxU32 heightBoxes, const NxVec3& boxDim, NxReal density)
    {
      NxActor** tower = new NxActor*[heightBoxes];
      NxReal spacing = 0.01f;
      NxVec3 boxPos = pos;

      for(NxU32 i = 0; i < heightBoxes; ++i)
      {
        tower[i] = CreateBox(boxPos, boxDim, density);
        boxPos.y += 2 * boxDim.y + spacing;
      }

      return tower;
    }

    NxQuat MyRagdoll::AnglesToQuat(const NxVec3& angles)
    {
      NxQuat quat;
      NxVec3 a;
      NxReal cr, cp, cy, sr, sp, sy, cpcy, spsy;
      a.x = (NxPi / (NxReal)360.0) * angles.x;  // Pitch
      a.y = (NxPi / (NxReal)360.0) * angles.y;  // Yaw
      a.z = (NxPi / (NxReal)360.0) * angles.z;  // Roll
      cy = NxMath::cos(a.z);
      cp = NxMath::cos(a.y);
      cr = NxMath::cos(a.x);
      sy = NxMath::sin(a.z);
      sp = NxMath::sin(a.y);
      sr = NxMath::sin(a.x);
      cpcy = cp * cy;
      spsy = sp * sy;
      quat.w = cr * cpcy + sr * spsy;
      quat.x = sr * cpcy - cr * spsy;
      quat.y = cr * sp * cy + sr * cp * sy;
      quat.z = cr * cp * sy - sr * sp * cy;
      return quat;
    }

    NxActor* MyRagdoll::CreateBoxGear(const NxVec3& pos, const NxReal minRadius, const NxReal maxRadius, const NxReal height, const NxU32 numTeeth, const NxReal density)
    {
      NxActorDesc actorDesc;
      NxBodyDesc bodyDesc;
      bodyDesc.solverIterationCount = 30;  // Fast-moving body, higher solver iteration count
      NxReal perimeter = 2 * NxPi * minRadius;
      NxReal toothWidth = (perimeter / (numTeeth * 2)) * (NxReal)0.75;
      NxReal toothHeight = height;
      NxReal toothDepth = maxRadius - minRadius;
      NxReal a, c, s;
      NxU32 i;
      NxVec3* verts = new NxVec3[numTeeth * 2];

      for(i = 0; i < numTeeth; ++i)
      {
        a = 360 * (i / (NxReal)numTeeth);
        c = NxMath::cos(a * (NxPi / 180)) * minRadius;
        s = NxMath::sin(a * (NxPi / 180)) * minRadius;
        verts[2 * i] = NxVec3(s, -height / 2, c);
        verts[2 * i + 1] = NxVec3(s, height / 2, c);
      }

      // Create descriptor for gear center convex mesh
      NxConvexMeshDesc centerDesc;
      centerDesc.numVertices      = numTeeth * 2;
      centerDesc.pointStrideBytes   = sizeof(NxVec3);
      centerDesc.points       = verts;
      centerDesc.flags        = NX_CF_COMPUTE_CONVEX;
      NxConvexShapeDesc centerShapeDesc;
      centerShapeDesc.localPose.t = NxVec3(0, 0, 0);
      NxInitCooking();

      if(0)
      {
        // Cooking from file
#ifdef WIN32
        bool status = NxCookConvexMesh(centerDesc, UserStream("c:\\tmp.bin", false));
        centerShapeDesc.meshData = gPhysicsSDK->createConvexMesh(UserStream("c:\\tmp.bin", true));
#endif
      }
      else
      {
        // Cooking from memory
        MemoryWriteBuffer buf;
        bool status = NxCookConvexMesh(centerDesc, buf);
        centerShapeDesc.meshData = gPhysicsSDK->createConvexMesh(MemoryReadBuffer(buf.data));
      }

      actorDesc.shapes.pushBack(&centerShapeDesc);
      NxBoxShapeDesc *boxDesc = new NxBoxShapeDesc[numTeeth];

      for(i = 0; i < numTeeth; ++i)
      {
        a = 360 * (i / (NxReal)numTeeth);
        c = NxMath::cos(a * (NxPi / 180)) * (minRadius + toothDepth / 2);
        s = NxMath::sin(a * (NxPi / 180)) * (minRadius + toothDepth / 2);
        boxDesc[i].dimensions.set(NxVec3(toothWidth / 2, toothHeight / 2, toothDepth / 2));
        NxQuat q = AnglesToQuat(NxVec3(0, a, 0));
        boxDesc[i].localPose.M.fromQuat(q);
        boxDesc[i].localPose.t = NxVec3(s, 0, c);
        actorDesc.shapes.pushBack(&boxDesc[i]);
      }

      if(density)
      {
        actorDesc.body = &bodyDesc;
        actorDesc.density = density;
      }
      else
      {
        actorDesc.body = NULL;
      }

      actorDesc.globalPose.t = pos;
      NxActor* actor = gScene->createActor(actorDesc);
      return actor;
    }

    NxActor* MyRagdoll::CreateWheel(const NxVec3& pos, const NxReal minRadius, const NxReal maxRadius, const NxReal height, const NxU32 numTeeth, const NxReal density)
    {
      NxActorDesc actorDesc;
      NxBodyDesc bodyDesc;
      NxReal perimeter = 2 * NxPi * minRadius;
      NxReal toothWidth = (perimeter / (numTeeth * 2)) * (NxReal)0.75;
      NxReal toothHeight = height;
      NxReal toothDepth = maxRadius - minRadius;
      NxReal a, c, s;
      NxU32 i;
      // WHEEL CENTER
      NxVec3* verts = new NxVec3[numTeeth * 2];

      for(i = 0; i < numTeeth; ++i)
      {
        a = 360 * (i / (NxReal)numTeeth);
        c = NxMath::cos(a * (NxPi / 180)) * minRadius;
        s = NxMath::sin(a * (NxPi / 180)) * minRadius;
        verts[2 * i] = NxVec3(s, -height / 2, c);
        verts[2 * i + 1] = NxVec3(s, height / 2, c);
      }

      // Create descriptor for gear center triangle mesh
      NxConvexMeshDesc centerDesc;
      centerDesc.numVertices      = numTeeth * 2;
      centerDesc.pointStrideBytes   = sizeof(NxVec3);
      centerDesc.points       = verts;
      centerDesc.flags        = NX_CF_COMPUTE_CONVEX;
      NxConvexShapeDesc centerShapeDesc;
      centerShapeDesc.localPose.t = NxVec3(0, 0, 0);
      NxInitCooking();

      if(0)
      {
        // Cooking from file
#ifdef WIN32
        bool status = NxCookConvexMesh(centerDesc, UserStream("c:\\tmp.bin", false));
        centerShapeDesc.meshData = gPhysicsSDK->createConvexMesh(UserStream("c:\\tmp.bin", true));
#endif
      }
      else
      {
        // Cooking from memory
        MemoryWriteBuffer buf;
        bool status = NxCookConvexMesh(centerDesc, buf);
        centerShapeDesc.meshData = gPhysicsSDK->createConvexMesh(MemoryReadBuffer(buf.data));
      }

      actorDesc.shapes.pushBack(&centerShapeDesc);

      // WHEEL LEFT EDGE
      for(i = 0; i < numTeeth; ++i)
      {
        a = 360 * (i / (NxReal)numTeeth);
        c = NxMath::cos(a * (NxPi / (NxReal)180)) * maxRadius;
        s = NxMath::sin(a * (NxPi / (NxReal)180)) * maxRadius;
        verts[2 * i] = NxVec3(s, -(NxReal)0.1, c);
        verts[2 * i + 1] = NxVec3(s, (NxReal)0.1, c);
      }

      // Create descriptor for gear left edge convex mesh
      NxConvexMeshDesc leftEdgeDesc;
      leftEdgeDesc.numVertices    = numTeeth * 2;
      leftEdgeDesc.pointStrideBytes = sizeof(NxVec3);
      leftEdgeDesc.points       = verts;
      leftEdgeDesc.flags        = NX_CF_COMPUTE_CONVEX;
      NxConvexShapeDesc leftEdgeShapeDesc;
      leftEdgeShapeDesc.localPose.t = NxVec3(0, -height / 2 - (NxReal)0.1, 0);
      NxInitCooking();

      if(0)
      {
        // Cooking from file
#ifdef WIN32
        bool status = NxCookConvexMesh(leftEdgeDesc, UserStream("c:\\tmp.bin", false));
        leftEdgeShapeDesc.meshData = gPhysicsSDK->createConvexMesh(UserStream("c:\\tmp.bin", true));
#endif
      }
      else
      {
        // Cooking from memory
        MemoryWriteBuffer buf;
        bool status = NxCookConvexMesh(leftEdgeDesc, buf);
        leftEdgeShapeDesc.meshData = gPhysicsSDK->createConvexMesh(MemoryReadBuffer(buf.data));
      }

      actorDesc.shapes.pushBack(&leftEdgeShapeDesc);

      // WHEEL RIGHT EDGE
      for(i = 0; i < numTeeth; ++i)
      {
        a = 360 * (i / (NxReal)numTeeth);
        c = NxMath::cos(a * (NxPi / 180)) * maxRadius;
        s = NxMath::sin(a * (NxPi / 180)) * maxRadius;
        verts[2 * i] = NxVec3(s, -(NxReal)0.1, c);
        verts[2 * i + 1] = NxVec3(s, (NxReal)0.1, c);
      }

      // Create descriptor for gear left edge triangle mesh
      NxConvexMeshDesc rightEdgeDesc;
      rightEdgeDesc.numVertices   = numTeeth * 2;
      rightEdgeDesc.pointStrideBytes  = sizeof(NxVec3);
      rightEdgeDesc.points      = verts;
      rightEdgeDesc.flags       = NX_CF_COMPUTE_CONVEX;
      NxConvexShapeDesc rightEdgeShapeDesc;
      rightEdgeShapeDesc.localPose.t = NxVec3(0, height / 2 + (NxReal)0.1, 0);
      NxInitCooking();

      if(0)
      {
        // Cooking from file
#ifdef WIN32
        bool status = NxCookConvexMesh(rightEdgeDesc, UserStream("c:\\tmp.bin", false));
        rightEdgeShapeDesc.meshData = gPhysicsSDK->createConvexMesh(UserStream("c:\\tmp.bin", true));
#endif
      }
      else
      {
        // Cooking from memory
        MemoryWriteBuffer buf;
        bool status = NxCookConvexMesh(rightEdgeDesc, buf);
        rightEdgeShapeDesc.meshData = gPhysicsSDK->createConvexMesh(MemoryReadBuffer(buf.data));
      }

      actorDesc.shapes.pushBack(&rightEdgeShapeDesc);
      NxBoxShapeDesc *boxDesc = new NxBoxShapeDesc[numTeeth];

      for(i = 0; i < numTeeth; ++i)
      {
        a = 360 * (i / (NxReal)numTeeth);
        c = NxMath::cos(a * (NxPi / 180)) * (minRadius + toothDepth / 2);
        s = NxMath::sin(a * (NxPi / 180)) * (minRadius + toothDepth / 2);
        boxDesc[i].dimensions.set(NxVec3(toothWidth / 2, toothHeight / 2, toothDepth / 2));
        NxQuat q = AnglesToQuat(NxVec3(0, a, 0));
        boxDesc[i].localPose.M.fromQuat(q);
        boxDesc[i].localPose.t = NxVec3(s, 0, c);
        actorDesc.shapes.pushBack(&boxDesc[i]);
      }

      if(density)
      {
        actorDesc.body = &bodyDesc;
        actorDesc.density = density;
      }
      else
      {
        actorDesc.body = NULL;
      }

      actorDesc.globalPose.t = pos;
      NxActor* actor = gScene->createActor(actorDesc);
      return actor;
    }

    NxActor* MyRagdoll::CreateFrame(const NxVec3& pos, const NxReal density)
    {
      NxActorDesc actorDesc;
      NxBodyDesc bodyDesc;
      NxBoxShapeDesc boxDesc1;
      boxDesc1.dimensions.set(8, (NxReal)0.5, (NxReal)0.05);
      boxDesc1.localPose.t = NxVec3(0, 0, (NxReal)1.7);
      actorDesc.shapes.pushBack(&boxDesc1);
      NxBoxShapeDesc boxDesc2;
      boxDesc2.dimensions.set(8, (NxReal)0.5, (NxReal)0.05);
      boxDesc2.localPose.t = NxVec3(0, 0, -(NxReal)1.7);
      actorDesc.shapes.pushBack(&boxDesc2);

      if(density)
      {
        actorDesc.body = &bodyDesc;
        actorDesc.density = density;
      }
      else
      {
        actorDesc.body = NULL;
      }

      actorDesc.globalPose.t = pos;
      return gScene->createActor(actorDesc);
    }

    NxActor* MyRagdoll::CreateStep(const NxVec3& pos, const NxVec3& boxDim, const NxReal density)
    {
      NxActorDesc actorDesc;
      NxBodyDesc bodyDesc;
      NxBoxShapeDesc boxDesc1;
      boxDesc1.dimensions.set(boxDim.x / 2, boxDim.y / 2, boxDim.z / 2);
      boxDesc1.localPose.t = NxVec3(0, 0, 0);
      actorDesc.shapes.pushBack(&boxDesc1);
      NxBoxShapeDesc boxDesc2;
      boxDesc2.dimensions.set(boxDim.x / 6, boxDim.y / 16, (NxReal)0.125);
      boxDesc2.localPose.t = NxVec3(0, 0, -(NxReal)0.1);
      actorDesc.shapes.pushBack(&boxDesc2);
      NxBoxShapeDesc boxDesc3;
      boxDesc3.dimensions.set((NxReal)0.1, boxDim.y / 2, (NxReal)0.5);
      boxDesc3.localPose.t = NxVec3(boxDim.x / 2, 0, (NxReal)(0.201 - 0.25));
      actorDesc.shapes.pushBack(&boxDesc3);
      NxBoxShapeDesc boxDesc4;
      boxDesc4.dimensions.set((NxReal)0.1, boxDim.y / 2, (NxReal)0.5);
      boxDesc4.localPose.t = NxVec3(-boxDim.x / 2, 0, (NxReal)(0.201 - 0.25));
      actorDesc.shapes.pushBack(&boxDesc4);

      if(density)
      {
        actorDesc.body = &bodyDesc;
        actorDesc.density = density;
      }
      else
      {
        actorDesc.body = NULL;
      }

      actorDesc.globalPose.t = pos;
      return gScene->createActor(actorDesc);
    }

    NxActor* MyRagdoll::CreateChassis(const NxVec3& pos, const NxVec3& boxDim, const NxReal density)
    {
      // Add a single-shape actor to the scene
      NxActorDesc actorDesc;
      NxBodyDesc bodyDesc;
      // Half-Pyramid
      NxVec3 verts[16] = {NxVec3(0, 4 * boxDim.y, -boxDim.z * (NxReal)0.5), NxVec3(-boxDim.x * (NxReal)0.75, 4 * boxDim.y, -boxDim.z * (NxReal)0.5), NxVec3(-boxDim.x * (NxReal)0.75, 4 * boxDim.y, boxDim.z * (NxReal)0.5), NxVec3(0, 4 * boxDim.y, boxDim.z * (NxReal)0.5),
                          NxVec3(boxDim.x, 2 * boxDim.y, -boxDim.z), NxVec3(-boxDim.x, 2 * boxDim.y, -boxDim.z), NxVec3(-boxDim.x, 2 * boxDim.y, boxDim.z), NxVec3(boxDim.x, 2 * boxDim.y, boxDim.z),
                          NxVec3(boxDim.x, boxDim.y, -boxDim.z), NxVec3(-boxDim.x, boxDim.y, -boxDim.z), NxVec3(-boxDim.x, boxDim.y, boxDim.z), NxVec3(boxDim.x, boxDim.y, boxDim.z),
                          NxVec3(boxDim.x * (NxReal)0.75, -boxDim.y, -boxDim.z * (NxReal)0.75), NxVec3(-boxDim.x * (NxReal)0.75, -boxDim.y, -boxDim.z * (NxReal)0.75), NxVec3(-boxDim.x * (NxReal)0.75, -boxDim.y, boxDim.z * (NxReal)0.75), NxVec3(boxDim.x * (NxReal)0.75, -boxDim.y, boxDim.z * (NxReal)0.75)
                         };
      // Create descriptor for triangle mesh
      NxConvexMeshDesc convexDesc;
      convexDesc.numVertices      = 16;
      convexDesc.pointStrideBytes   = sizeof(NxVec3);
      convexDesc.points       = verts;
      convexDesc.flags        = NX_CF_COMPUTE_CONVEX;
      // The actor has one shape, a convex mesh
      NxConvexShapeDesc convexShapeDesc;
      convexShapeDesc.localPose.t = NxVec3(0, boxDim.y, 0);
      NxInitCooking();

      if(0)
      {
        // Cooking from file
#ifdef WIN32
        bool status = NxCookConvexMesh(convexDesc, UserStream("c:\\tmp.bin", false));
        convexShapeDesc.meshData = gPhysicsSDK->createConvexMesh(UserStream("c:\\tmp.bin", true));
#endif
      }
      else
      {
        // Cooking from memory
        MemoryWriteBuffer buf;
        bool status = NxCookConvexMesh(convexDesc, buf);
        convexShapeDesc.meshData = gPhysicsSDK->createConvexMesh(MemoryReadBuffer(buf.data));
      }

      actorDesc.shapes.pushBack(&convexShapeDesc);

      if(density)
      {
        actorDesc.body = &bodyDesc;
        actorDesc.density = density;
      }
      else
      {
        actorDesc.body = NULL;
      }

      actorDesc.globalPose.t = pos;
      NxActor* actor = gScene->createActor(actorDesc);
      return actor;
    }

    NxActor* MyRagdoll::CreateTurret(const NxVec3& pos, const NxVec3& boxDim, const NxReal density)
    {
      // Add a single-shape actor to the scene
      NxActorDesc actorDesc;
      NxBodyDesc bodyDesc;
      // Half-Pyramid
      NxVec3 verts[16] =
      {
        NxVec3(boxDim.x * (NxReal)0.25, 6 * boxDim.y, -boxDim.z * (NxReal)0.5), NxVec3(-boxDim.x * (NxReal)0.75, 6 * boxDim.y, -boxDim.z * (NxReal)0.5), NxVec3(-boxDim.x * (NxReal)0.75, 6 * boxDim.y, boxDim.z * (NxReal)0.5), NxVec3(boxDim.x * (NxReal)0.25, 6 * boxDim.y, boxDim.z * (NxReal)0.5),
        NxVec3(boxDim.x * (NxReal)0.5, (NxReal)5.5 * boxDim.y, -boxDim.z * (NxReal)0.65), NxVec3(-boxDim.x * (NxReal)0.8, (NxReal)5.5 * boxDim.y, -boxDim.z * (NxReal)0.65), NxVec3(-boxDim.x * (NxReal)0.8, (NxReal)5.5 * boxDim.y, boxDim.z * (NxReal)0.65), NxVec3(boxDim.x * (NxReal)0.5, (NxReal)5.5 * boxDim.y, boxDim.z * (NxReal)0.65),
        NxVec3(boxDim.x * (NxReal)0.5, (NxReal)4.5 * boxDim.y, -boxDim.z * (NxReal)0.65), NxVec3(-boxDim.x * (NxReal)0.8, (NxReal)4.5 * boxDim.y, -boxDim.z * (NxReal)0.65), NxVec3(-boxDim.x * (NxReal)0.8, (NxReal)4.5 * boxDim.y, boxDim.z * (NxReal)0.65), NxVec3(boxDim.x * (NxReal)0.5, (NxReal)4.5 * boxDim.y, boxDim.z * (NxReal)0.65),
        NxVec3(boxDim.x * (NxReal)0.25, 4 * boxDim.y, -boxDim.z * (NxReal)0.5), NxVec3(-boxDim.x * (NxReal)0.75, 4 * boxDim.y, -boxDim.z * (NxReal)0.5), NxVec3(-boxDim.x * (NxReal)0.75, 4 * boxDim.y, boxDim.z * (NxReal)0.5), NxVec3(boxDim.x * (NxReal)0.25, 4 * boxDim.y, boxDim.z * (NxReal)0.5),
      };
      // Create descriptor for convex mesh
      NxConvexMeshDesc convexDesc;
      convexDesc.numVertices      = 16;
      convexDesc.pointStrideBytes   = sizeof(NxVec3);
      convexDesc.points       = verts;
      convexDesc.flags        = NX_CF_COMPUTE_CONVEX;
      // The actor has one shape, a convex mesh
      NxConvexShapeDesc convexShapeDesc;
      convexShapeDesc.localPose.t = NxVec3(0, boxDim.y, 0);
      NxInitCooking();

      if(0)
      {
        // Cooking from file
#ifdef WIN32
        bool status = NxCookConvexMesh(convexDesc, UserStream("c:\\tmp.bin", false));
        convexShapeDesc.meshData = gPhysicsSDK->createConvexMesh(UserStream("c:\\tmp.bin", true));
#endif
      }
      else
      {
        // Cooking from memory
        MemoryWriteBuffer buf;
        bool status = NxCookConvexMesh(convexDesc, buf);
        convexShapeDesc.meshData = gPhysicsSDK->createConvexMesh(MemoryReadBuffer(buf.data));
      }

      actorDesc.shapes.pushBack(&convexShapeDesc);

      if(density)
      {
        actorDesc.body = &bodyDesc;
        actorDesc.density = density;
      }
      else
      {
        actorDesc.body = NULL;
      }

      actorDesc.globalPose.t = pos;
      NxActor* actor = gScene->createActor(actorDesc);
      return actor;
    }

    NxActor* MyRagdoll::CreateCannon(const NxVec3& pos, const NxVec3& boxDim, const NxReal density)
    {
      // Add a single-shape actor to the scene
      NxActorDesc actorDesc;
      NxBodyDesc bodyDesc;
      // Half-Pyramid
      NxVec3 verts[8] =
      {
        NxVec3(boxDim.x * 2, (NxReal)5.4 * boxDim.y, -boxDim.z * (NxReal)0.1), NxVec3(boxDim.x * (NxReal)0.5, (NxReal)5.4 * boxDim.y, -boxDim.z * (NxReal)0.1), NxVec3(boxDim.x * (NxReal)0.5, (NxReal)5.4 * boxDim.y, boxDim.z * (NxReal)0.1), NxVec3(boxDim.x * 2, (NxReal)5.4 * boxDim.y, boxDim.z * (NxReal)0.1),
        NxVec3(boxDim.x * 2, (NxReal)4.6 * boxDim.y, -boxDim.z * (NxReal)0.1), NxVec3(boxDim.x * (NxReal)0.5, (NxReal)4.6 * boxDim.y, -boxDim.z * (NxReal)0.1), NxVec3(boxDim.x * (NxReal)0.5, (NxReal)4.6 * boxDim.y, boxDim.z * (NxReal)0.1), NxVec3(boxDim.x * 2, (NxReal)4.6 * boxDim.y, boxDim.z * (NxReal)0.1),
      };
      // Create descriptor for triangle mesh
      NxConvexMeshDesc convexDesc;
      convexDesc.numVertices      = 8;
      convexDesc.pointStrideBytes   = sizeof(NxVec3);
      convexDesc.points       = verts;
      convexDesc.flags        = NX_CF_COMPUTE_CONVEX;
      // The actor has one shape, a convex mesh
      NxConvexShapeDesc convexShapeDesc;
      convexShapeDesc.localPose.t = NxVec3(0, boxDim.y, 0);
      NxInitCooking();

      if(0)
      {
        // Cooking from file
#ifdef WIN32
        bool status = NxCookConvexMesh(convexDesc, UserStream("c:\\tmp.bin", false));
        convexShapeDesc.meshData = gPhysicsSDK->createConvexMesh(UserStream("c:\\tmp.bin", true));
#endif
      }
      else
      {
        // Cooking from memory
        MemoryWriteBuffer buf;
        bool status = NxCookConvexMesh(convexDesc, buf);
        convexShapeDesc.meshData = gPhysicsSDK->createConvexMesh(MemoryReadBuffer(buf.data));
      }

      actorDesc.shapes.pushBack(&convexShapeDesc);

      if(density)
      {
        actorDesc.body = &bodyDesc;
        actorDesc.density = density;
      }
      else
      {
        actorDesc.body = NULL;
      }

      actorDesc.globalPose.t = pos;
      NxActor* actor = gScene->createActor(actorDesc);
      return actor;
    }

    NxActor* MyRagdoll::CreateBlade(const NxVec3& pos, const NxVec3& boxDim, const NxReal mass)
    {
      // Add a single-shape actor to the scene
      NxActorDesc actorDesc;
      NxBodyDesc bodyDesc;
      // The actor has one shape, a box
      NxBoxShapeDesc boxDesc;
      boxDesc.dimensions.set(boxDim.x, boxDim.y, boxDim.z);
      boxDesc.localPose.t = NxVec3(0, boxDim.y, 0);
      actorDesc.shapes.pushBack(&boxDesc);

      if(mass)
      {
        bodyDesc.mass = mass;
        actorDesc.body = &bodyDesc;
      }
      else
      {
        actorDesc.body = NULL;
      }

      actorDesc.globalPose.t = pos;
      return gScene->createActor(actorDesc);
    }

    NxActor* MyRagdoll::CreateBall(const NxVec3& pos, const NxReal radius, const NxReal mass)
    {
      // Add a single-shape actor to the scene
      NxActorDesc actorDesc;
      NxBodyDesc bodyDesc;
      // The actor has one shape, a sphere
      NxSphereShapeDesc sphereDesc;
      sphereDesc.radius = radius;
      sphereDesc.localPose.t = NxVec3(0, radius, 0);
      actorDesc.shapes.pushBack(&sphereDesc);

      if(mass)
      {
        bodyDesc.mass = mass;
        actorDesc.body = &bodyDesc;
      }
      else
      {
        actorDesc.body = NULL;
      }

      actorDesc.globalPose.t = pos;
      return gScene->createActor(actorDesc);
    }

    void MyRagdoll::SetActorCollisionGroup(NxActor* actor, NxCollisionGroup group)
    {
      NxShape*const* shapes = actor->getShapes();
      NxU32 nShapes = actor->getNbShapes();

      while(nShapes--)
      {
        shapes[nShapes]->setGroup(group);
      }
    }

    void MyRagdoll::SetActorMaterial(NxActor *actor, NxMaterialIndex index)
    {
      NxU32 nbShapes = actor->getNbShapes();
      NxShape*const* shapes = actor->getShapes();

      while(nbShapes--)
      {
        shapes[nbShapes]->setMaterial(index);
      }
    }

    void MyRagdoll::PageInHardwareMeshes(NxScene* scene)
    {
      // Page in the hardware meshes
      for(NxU32 i = 0; i < scene->getNbActors(); ++i)
      {
        NxActor* actor = scene->getActors()[i];
        NxShape*const* shapes = actor->getShapes();

        for(NxU32 s = 0; s < actor->getNbShapes(); ++s)
        {
          NxShape* shape = (NxShape*)(shapes[s]);

          if(shape->is(NX_SHAPE_MESH))
          {
            NxTriangleMeshShape* triMeshShape = (NxTriangleMeshShape *)shape;
            NxTriangleMesh& triMesh = triMeshShape->getTriangleMesh();
            NxU32 pageCnt = triMesh.getPageCount();

            for(NxU32 t = 0; t < pageCnt; ++t)
            {
              if(!triMeshShape->isPageInstanceMapped(t))
              {
                bool ok = triMeshShape->mapPageInstance(t);
                assert(ok);
              }
            }
          }
        }
      }
    }


    void MyRagdoll::SwitchCoordinateSystem(NxScene* scene, NxMat34 mat)
    {
      NxU32 nbActors = scene->getNbActors();
      NxActor** actors = scene->getActors();
      NxMat33 roll2;
      roll2.rotX(-NxPi * 0.5f);
      NxMat34 invMat;
      mat.getInverse(invMat);
      bool bStaticActor;

      // Adjust actors' positions and orientations
      for(NxU32 i = 0; i < nbActors; ++i)
      {
        NxActor* actor = actors[i];
        NxVec3 pos = actor->getGlobalPosition();
        pos = mat * pos;
        bStaticActor = false;

        if(!actor->isDynamic())
        {
          bStaticActor = true;
        }

        if(!bStaticActor)
        {
          actor->setGlobalPosition(pos);
        }

        // Adjust shape's positions and orientations within actors
        NxU32 nbShapes = actor->getNbShapes();
        NxShape*const* shape = actor->getShapes();
        NxActorDesc newActorDesc;

        if(bStaticActor)
        {
          actor->saveToDesc(newActorDesc);
        }

        for(NxU32 j = 0; j < nbShapes; ++j)
        {
          // Transform the local position of each shape
          NxVec3 pos = shape[j]->getLocalPosition();
          pos = mat * pos;
          shape[j]->setLocalPosition(pos);

          if(NxPlaneShape* planeShape = shape[j]->isPlane())
          {
            NxPlaneShapeDesc planeDesc;
            planeShape->saveToDesc(planeDesc);
            NxReal y = planeDesc.normal.y;
            planeDesc.normal.y = planeDesc.normal.z;
            planeDesc.normal.z = y;

            if(bStaticActor)
            {
              newActorDesc.shapes.push_back(&planeDesc);
            }

            planeShape->setPlane(planeDesc.normal, planeDesc.d);
          }

          if(NxBoxShape* boxShape = shape[j]->isBox())
          {
            NxBoxShapeDesc boxDesc;
            boxShape->saveToDesc(boxDesc);
            NxReal y = boxDesc.dimensions.y;
            boxDesc.dimensions.y = boxDesc.dimensions.z;
            boxDesc.dimensions.z = y;
            boxShape->setDimensions(boxDesc.dimensions);

            if(bStaticActor)
            {
              newActorDesc.shapes.push_back(&boxDesc);
            }
          }

          if(NxCapsuleShape* capsuleShape = shape[j]->isCapsule())
          {
            NxMat33 mat = shape[j]->getGlobalOrientation();
            mat = roll2 * mat;
            shape[j]->setGlobalOrientation(mat);
          }

          if(NxConvexShape* convexShape = shape[j]->isConvexMesh())
          {
            // TRANSFORM AND RE-COOK THE CONVEX MESH DATA
            NxConvexShapeDesc convexShapeDesc;
            convexShape->saveToDesc(convexShapeDesc);
            // Transform the mesh data to the new coordinate system
            NxConvexMeshDesc convexDesc;
            convexShape->getConvexMesh().saveToDesc(convexDesc);
            // TRANSFORM POINTS HERE
            NxVec3* verts = (NxVec3*)convexDesc.points;

            for(NxU32 i = 0; i < convexDesc.numVertices; ++i)
            {
              NxVec3 pos = verts[i];
              pos = mat * pos;
              verts[i] = pos;
            }

            // Cook the new mesh data
            NxInitCooking();

            if(0)
            {
              // Cooking from file
#ifdef WIN32
              bool status = NxCookConvexMesh(convexDesc, UserStream("c:\\tmp.bin", false));
              convexShapeDesc.meshData = gPhysicsSDK->createConvexMesh(UserStream("c:\\tmp.bin", true));
#endif
            }
            else
            {
              // Cooking from memory
              MemoryWriteBuffer buf;
              bool status = NxCookConvexMesh(convexDesc, buf);
              convexShapeDesc.meshData = gPhysicsSDK->createConvexMesh(MemoryReadBuffer(buf.data));
            }

            if(bStaticActor)
            {
              newActorDesc.shapes.push_back(&convexShapeDesc);
            }
            else
            {
              actor->createShape(convexShapeDesc);
              NxActorDesc actorDesc;
              actor->saveToDesc(actorDesc);
              actorDesc.shapes.pushBack(&convexShapeDesc);
              shape[j]->userData = (void*) - 1; // Schedule for deletion
            }
          }

          if(NxTriangleMeshShape* meshShape = shape[j]->isTriangleMesh())
          {
            // TRANSFORM AND RE-COOK THE TRIANGLE MESH DATA
            NxTriangleMeshShapeDesc meshShapeDesc;
            meshShape->saveToDesc(meshShapeDesc);
            // Transform the mesh data to the new coordinate system
            NxTriangleMeshDesc meshDesc;
            meshShape->getTriangleMesh().saveToDesc(meshDesc);
            // TRANSFORM POINTS HERE
            NxVec3* verts = (NxVec3*)meshDesc.points;

            for(NxU32 i = 0; i < meshDesc.numVertices; ++i)
            {
              NxVec3 pos = verts[i];
              pos = mat * pos;
              verts[i] = pos;
            }

            // Cook the new mesh data
            NxInitCooking();

            if(0)
            {
              // Cooking from file
#ifdef WIN32
              bool status = NxCookTriangleMesh(meshDesc, UserStream("c:\\tmp.bin", false));
              meshShapeDesc.meshData = gPhysicsSDK->createTriangleMesh(UserStream("c:\\tmp.bin", true));
#endif
            }
            else
            {
              // Cooking from memory
              MemoryWriteBuffer buf;
              bool status = NxCookTriangleMesh(meshDesc, buf);
              meshShapeDesc.meshData = gPhysicsSDK->createTriangleMesh(MemoryReadBuffer(buf.data));
            }

            if(bStaticActor)
            {
              newActorDesc.shapes.push_back(&meshShapeDesc);
            }
            else
            {
              actor->createShape(meshShapeDesc);
            }

            NxActorDesc actorDesc;
            actor->saveToDesc(actorDesc);
            actorDesc.shapes.pushBack(&meshShapeDesc);
            shape[j]->userData = (void*) - 1; // Schedule for deletion
          }
        }

        if(bStaticActor)
        {
          newActorDesc.globalPose.t = pos;
          scene->createActor(newActorDesc);
          scene->releaseActor(*actor);
        }

        //    actor->updateMassFromShapes(0,actor->getMass());
      }

      nbActors = scene->getNbActors();
      actors = scene->getActors();

      // Remove convex shapes scheduled for deletion
      // Update actor masses
      for(NxU32 i = 0; i < nbActors; ++i)
      {
        NxActor* actor = actors[i];
        // Release convex shapes scheduled for deletion
        NxU32 nbShapes = actor->getNbShapes();
        NxShape*const* shape = actor->getShapes();

        for(NxU32 j = 0; j < nbShapes; j++)
        {
          if(shape[j]->userData == (void*) - 1)
          {
            if(actor->isDynamic())
            {
              actor->releaseShape(*shape[j]);
            }

            shape[j]->userData = 0;
          }
        }

        if(actor->isDynamic())
        {
          actor->updateMassFromShapes(0, actor->getMass());
        }
      }
    }
};