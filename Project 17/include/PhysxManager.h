#pragma once

#include "OgreTerrain.h"

#include "NxPhysics.h"
#include "NxCooking.h"
#include "NxStream.h"
#include "NXU_Streaming.h"
#include "NXU_helper.h"
#include "PhysX/UserAllocator.h"
#include "PhysX/MyCharacterController.h"

using namespace std;
using namespace NXU;
using namespace Ogre;

class UserNotify: public NXU_userNotify
{
  public:
    std::vector<NxActor*> actors;
  public:

    virtual void  NXU_notifyJoint(NxJoint *joint, const char *userProperties) {};

    virtual void  NXU_notifyActor(NxActor *actor, const char *userProperties)
    {
      actors.push_back(actor);
    };

    virtual void  NXU_notifyCloth(NxCloth *cloth, const char *userProperties) {};

    virtual void  NXU_notifyFluid(NxFluid *fluid, const char *userProperties) {};

    virtual void  NXU_notifyTriangleMesh(NxTriangleMesh *t, const char *userProperties) {};

    virtual void  NXU_notifyConvexMesh(NxConvexMesh *c, const char *userProperties) {};

    virtual void  NXU_notifyClothMesh(NxClothMesh *t, const char *userProperties) {};

    virtual void  NXU_notifyCCDSkeleton(NxCCDSkeleton *t, const char *userProperties) {};

    virtual void  NXU_notifyHeightField(NxHeightField *t, const char *userProperties) {};

    virtual bool  NXU_preNotifyJoint(NxJointDesc &joint, const char *userProperties)
    {
      return true;
    };

    virtual bool  NXU_preNotifyActor(NxActorDesc &actor, const char *userProperties)
    {
      for(NxU32 i = 0; i < actor.shapes.size(); ++i)
      {
        NxShapeDesc *s = actor.shapes[i];
        s->shapeFlags |= NX_SF_VISUALIZATION; // make sure the shape visualization flags are on so we can see things!
      }

      return true;
    };

    virtual bool  NXU_preNotifyTriangleMesh(NxTriangleMeshDesc &t, const char *userProperties)
    {
      return true;
    };
    virtual bool  NXU_preNotifyConvexMesh(NxConvexMeshDesc &t, const char *userProperties)
    {
      return true;
    };

    virtual bool  NXU_preNotifyClothMesh(NxClothMeshDesc &t, const char *userProperties)
    {
      return true;
    };

    virtual bool  NXU_preNotifyCCDSkeleton(NxSimpleTriangleMesh &t, const char *userProperties)
    {
      return true;
    };

};

class PhysxManager
{
  public:
    SceneManager  *mSceneMgr;
    NxPhysicsSDK  *PxSDK;   //ядро физикса
    NxScene     *PxScene; //физическая сцена
    UserNotify    NXU_Notify;

    NxMaterial* defaultMaterial;
    NxMaterialDesc materialDesc;

    NxActor       *mTerrainActor;                     //актор терана
    NxHeightField   *heightField;                     //хейтфилд
    NxHeightFieldDesc *heightFieldDesc;                   //дескриптор хейдфилда

    //структура для совмещения графических и физических объектов
    struct Atom
    {
      NxActor   *actor;
      SceneNode *node;
      String    name;
    };

    //вектор структур Atom
    std::vector<Atom> atoms; //вектор структур Атом

    PhysxManager::PhysxManager(SceneManager *scene)
    {
      mSceneMgr = scene;
      //создаём сдк
      PxSDK = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION);

      if(!PxSDK)
      {
        Ogre::LogManager::getSingleton().logMessage("PhysxSDK initialization fail");
        return;
      }

      //для дебагера
      PxSDK->getFoundationSDK().getRemoteDebugger()->connect("localhost", 5425);
      PxSDK->setParameter(NX_VISUALIZATION_SCALE, 1);
      PxSDK->setParameter(NX_VISUALIZE_COLLISION_SHAPES, 1);
      PxSDK->setParameter(NX_VISUALIZE_ACTOR_AXES, 1);
      //создаём дескриптор сцены
      NxSceneDesc sceneDesc;
      sceneDesc.gravity.set(0, -98, 0);        //гравитация
      sceneDesc.simType = NX_SIMULATION_HW;     //юзать хардвар. для софтвар NX_SIMULATION_SW Для хардвар -//-_HW
      //создаём сцену
      PxScene = PxSDK->createScene(sceneDesc);

      if(!PxScene)
      {
        sceneDesc.simType     = NX_SIMULATION_SW;
        PxScene = PxSDK->createScene(sceneDesc);

        if(!PxScene)
        {
          Ogre::LogManager::getSingleton().logMessage("PhysxScene initialization fail");
          return;
        }
      }

      //параметры сдк
      PxSDK->setParameter(NX_SKIN_WIDTH, 0.01f);     //толщина поверхностей (1см)
      //создаём дефолтовый материал
      defaultMaterial = PxScene->getMaterialFromIndex(1);
      defaultMaterial->setRestitution(0.0001);       //сопротивление. (энергия потеряная при столкновении)
      defaultMaterial->setStaticFriction(100.0);
      defaultMaterial->setDynamicFriction(100.0);
      defaultMaterial->setFlags(NX_MF_ANISOTROPIC);
      defaultMaterial->setDynamicFrictionV(100.0f);
      defaultMaterial->setStaticFrictionV(100.0f);
      defaultMaterial->setDirOfAnisotropy(NxVec3(0, -1, 0));
    };

    NxScene* getPxScene()
    {
      return PxScene;
    };

    void PhysxManager::destroyAtom(String name)
    {
      if(!atoms.empty())
      {
        for(std::vector<Atom>::iterator iter = atoms.begin(); iter != atoms.end(); ++iter)
        {
          Atom *obj = &(*iter);

          if(obj->name == name)
          {
            PxScene->releaseActor(*obj->actor);
            mSceneMgr->destroySceneNode(obj->node);
            atoms.erase(iter);
          }

          delete obj;
        }
      }
    };

    void PhysxManager::destroyAtom(NxActor *actor)
    {
      if(!atoms.empty())
      {
        for(std::vector<Atom>::iterator iter = atoms.begin(); iter != atoms.end(); ++iter)
        {
          Atom *obj = &(*iter);

          if(obj->actor == actor)
          {
            PxScene->releaseActor(*obj->actor);
            mSceneMgr->destroySceneNode(obj->node);
            atoms.erase(iter);
          }

          delete obj;
        }
      }
    };

    void PhysxManager::updateAllAtoms(NxReal time)
    {
      //обновляем сцену
      PxScene->simulate(time);
      PxScene->flushStream();
      PxScene->fetchResults(NX_ALL_FINISHED, true);

      //обновляем все атомы
      for(unsigned int i = 0; i < atoms.size(); ++i)
      {
        if(atoms[i].actor && atoms[i].node)
        {
          atoms[i].node->setPosition(toOgre(atoms[i].actor->getGlobalPosition()));
          atoms[i].node->setOrientation(toOgre(atoms[i].actor->getGlobalOrientationQuat()));
        }
      }
    };

    NxActor* CookeTerrain(Ogre::Terrain *terr)
    {
      NxHeightFieldDesc heightFieldDesc;
      heightFieldDesc.nbColumns = terr->getSize();
      heightFieldDesc.nbRows = terr->getSize();
      heightFieldDesc.convexEdgeThreshold = 0;
      heightFieldDesc.thickness = 0;
      heightFieldDesc.samples = new NxU32[(terr->getSize()) * (terr->getSize())];
      heightFieldDesc.sampleStride = sizeof(NxU32);
      NxReal normMin = -32768.0f;
      NxReal normMax = 32767.0f;
      char* currentByte = (char*)heightFieldDesc.samples;

      for(NxU32 row = 0; row < terr->getSize(); ++row)
      {
        unsigned int tess = 1;

        for(NxU32 col = 0; col < terr->getSize(); ++col)
        {
          NxHeightFieldSample* currentSample = (NxHeightFieldSample*)currentByte;
          currentSample->height = (NxI16)((((*terr->getHeightData(row, terr->getSize() - 1 - col)) - terr->getMinHeight()) / (terr->getMaxHeight() - terr->getMinHeight())) * (normMax - normMin) + normMin);
          currentSample->tessFlag = tess;

          if(tess == 1)
          {
            tess = 0;
          }
          else
          {
            tess = 1;
          }

          currentByte += heightFieldDesc.sampleStride;
        }
      }

      NxHeightField* heightField = PxSDK->createHeightField(heightFieldDesc);
      delete [] heightFieldDesc.samples;
      NxHeightFieldShapeDesc heightFieldShapeDesc;
      heightFieldShapeDesc.heightField = heightField;
      heightFieldShapeDesc.heightScale = (terr->getMaxHeight() - terr->getMinHeight()) / (normMax - normMin);
      heightFieldShapeDesc.shapeFlags = NX_SF_FEATURE_INDICES;
      heightFieldShapeDesc.rowScale = terr->getWorldSize() / NxReal(terr->getSize() - 1);
      heightFieldShapeDesc.columnScale = terr->getWorldSize() / NxReal(terr->getSize() - 1);
      heightFieldShapeDesc.materialIndexHighBits = 0;
      heightFieldShapeDesc.holeMaterial = 2;
      NxActorDesc actorDesc;
      assert(heightFieldShapeDesc.isValid());
      actorDesc.shapes.pushBack(&heightFieldShapeDesc);
      actorDesc.body = NULL;
      actorDesc.globalPose.t.setx(terr->getPosition().x - terr->getWorldSize() / 2);
      actorDesc.globalPose.t.setz(terr->getPosition().z - terr->getWorldSize() / 2);
      actorDesc.globalPose.t.sety((terr->getMaxHeight() + terr->getMinHeight()) / 2);
      assert(actorDesc.isValid());
      NxActor* actor = PxScene->createActor(actorDesc);
      SetActorCollisionGroup(actor, GROUP_COLLIDABLE_NON_PUSHABLE);
      return actor;
    }

    void SetActorCollisionGroup(NxActor *actor, NxCollisionGroup group)
    {
      NxU32 nbShapes = actor->getNbShapes();
      NxShape*const* shapes = actor->getShapes();

      while(nbShapes--)
      {
        shapes[nbShapes]->setGroup(group);
      }
    }


    //хейтфилд создаётся по хейтмапу ептыть работает.
    void PhysxManager::CreateTerrain()
    {
      //присваиваем значения
      ulong mPageSize  = 257;
      //Загружаем карту высот
      Image mImage;
      mImage.load("isl_h.png", ResourceGroupManager::getSingleton().getWorldResourceGroupName());
      NxMaterialIndex   mMaterialIndex = defaultMaterial->getMaterialIndex(); //материал
      NxActorDesc ActorDesc;
      //создаём хейтфилд
      heightFieldDesc = new NxHeightFieldDesc;
      heightFieldDesc->nbColumns    = mPageSize;
      heightFieldDesc->nbRows     = mPageSize;
      heightFieldDesc->verticalExtent = -1000;
      //    heightFieldDesc->convexEdgeThreshold = 0;
      heightFieldDesc->samples    = new NxU32[mPageSize * mPageSize];
      heightFieldDesc->sampleStride = sizeof(NxU32);
      char* currentByte = (char*)heightFieldDesc->samples;     //эээмм текущий сэмпли типа?

      //назначаем высоту для каждого сэмпла
      for(ulong row = 0; row < mPageSize; ++row)
      {
        for(ulong column = 0; column < mPageSize; ++column)   //цикл по сэмплам
        {
          ColourValue colval = mImage.getColourAt(row, column, 0);
          float height = (colval.r * 32767);
          //        LogManager::getSingletonPtr()->logMessage(Ogre::StringConverter::toString(height));
          NxHeightFieldSample* currentSample = (NxHeightFieldSample*)currentByte;
          currentSample->height = height;
          currentSample->materialIndex0 = mMaterialIndex;
          currentSample->materialIndex1 = mMaterialIndex;
          currentSample->tessFlag = 0;
          currentByte += heightFieldDesc->sampleStride;
        }
      }

      heightField = PxScene->getPhysicsSDK().createHeightField(*heightFieldDesc);
      NxHeightFieldShapeDesc heightFieldShapeDesc;
      heightFieldShapeDesc.heightField  = heightField;
      heightFieldShapeDesc.shapeFlags   = NX_SF_FEATURE_INDICES | NX_SF_VISUALIZATION;
      heightFieldShapeDesc.group  = 1;
      heightFieldShapeDesc.heightScale  = (1500.0f / 32767);
      heightFieldShapeDesc.rowScale   = 40000 / 256;
      heightFieldShapeDesc.columnScale  = 40000 / 256;
      //    heightFieldShapeDesc.meshFlags  = NX_MESH_SMOOTH_SPHERE_COLLISIONS;
      heightFieldShapeDesc.materialIndexHighBits = 0;
      heightFieldShapeDesc.holeMaterial = 2;
      ActorDesc.shapes.pushBack(&heightFieldShapeDesc);
      ActorDesc.globalPose.t.setx(0);
      ActorDesc.globalPose.t.setz(0);
      ActorDesc.globalPose.t.sety(0);
      mTerrainActor = PxScene->createActor(ActorDesc);
      mTerrainActor->userData = this;
      mTerrainActor->setGroup(4);
      SceneNode* terrainNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("terrain");
    }


    void PhysxManager::createBox(Vector3 pos, Vector3 scale, bool Static)
    {
      NxActor     *actor;
      NxActorDesc   actorDesc;
      NxBodyDesc    bodyDesc;
      NxBoxShapeDesc  boxDesc;
      boxDesc.dimensions.set(toNx(scale));

      if(Static)
      {
        boxDesc.group = GROUP_COLLIDABLE_NON_PUSHABLE;
      }
      else
      {
        boxDesc.group = GROUP_COLLIDABLE_PUSHABLE;
      }

      actorDesc.shapes.push_back(&boxDesc);
      NxVec3 gColorStatic(0.0f, 0.0f, 0.0f);
      actorDesc.userData = &gColorStatic;
      actorDesc.globalPose.t = toNx(pos);
      actor = PxScene->createActor(actorDesc);
    };

    void PhysxManager::addStaticBox(String name, Vector3 pos)
    {
      Atom      newAtom;
      NxActorDesc   actorDesc;
      NxBodyDesc    bodyDesc;
      NxBoxShapeDesc  boxDesc;
      Entity *ent = mSceneMgr->createEntity(name, "crate.mesh");
      ent->setMaterialName("Crate");
      newAtom.node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
      newAtom.node->attachObject(ent);
      newAtom.node->scale(50, 50, 50);
      newAtom.node->setPosition(pos);
      newAtom.name = name;
      boxDesc.dimensions.set(toNx(ent->getBoundingBox().getHalfSize() * 50));
      boxDesc.group     = GROUP_COLLIDABLE_NON_PUSHABLE;
      boxDesc.materialIndex = defaultMaterial->getMaterialIndex();
      actorDesc.shapes.push_back(&boxDesc);
      actorDesc.globalPose.t = toNx(pos);
      NxVec3 gColorStatic(0.0f, 0.0f, 0.0f);
      actorDesc.userData = &gColorStatic;
      newAtom.actor = PxScene->createActor(actorDesc);
      atoms.push_back(newAtom);
    };

    NxActor* LoadNxbActor(String filename)
    {
      //загружаем физ тело из файла
      NXU::NxuPhysicsCollection *c = NXU::loadCollection(filename.c_str(), NXU::NXU_FileType::FT_BINARY);
      NXU::instantiateCollection(c, *PxSDK, PxScene, 0, &NXU_Notify);
      return NXU_Notify.actors[0];
    };

    NxActor* LoadXmlShape(String filename)
    {
      //загружаем физ тело из файла
      NXU::NxuPhysicsCollection *c = NXU::loadCollection(filename.c_str(), NXU::NXU_FileType::FT_XML);
      NXU::instantiateCollection(c, *PxSDK, PxScene, 0, &NXU_Notify);
      return NXU_Notify.actors[0];
    };

    void SaveXmlShape(String filename, NxActor *a) // работоспособность не гарантируется=)
    {
      //загружаем физ тело из файла
      NXU::NxuPhysicsCollection *c;
      NXU::addActor(*c, *a);
      NXU::instantiateCollection(c, *PxSDK, PxScene, 0, &NXU_Notify);
      NXU::saveCollection(c, filename.c_str(), NXU::NXU_FileType::FT_XML);
    };

    void createShape(String name, String meshFile, Vector3 position, Quaternion orientation)
    {
      NxActor *mActor;
      mActor = LoadXmlShape(meshFile + ".xml");
      mActor->getShapes()[0]->setGroup(GROUP_COLLIDABLE_NON_PUSHABLE);
      mActor->setGlobalPosition(toNx(position));
      mActor->setGlobalOrientationQuat(toNx(orientation, true));
      addAtom(name, mActor, mSceneMgr->getRootSceneNode());
      LogManager::getSingleton().logMessage("done");
    }

    NxTriangleMeshShapeDesc* PhysxManager::createFullMeshShapeFromModel(Entity *pEntity,  Vector3 vScale)
    {
      NxTriangleMeshShapeDesc *lNxTriangleMeshDesc = new NxTriangleMeshShapeDesc();
      Ogre::String lMeshName = pEntity->getMesh()->getName();
      lNxTriangleMeshDesc->setToDefault();
      unsigned int mVertexCount = 0;
      unsigned int mIndexCount = 0;
      size_t vertex_count;
      Ogre::Vector3* vertices;
      size_t index_count;
      unsigned long* indices;
      bool added_shared = false;
      bool hasWarning = false;
      vertex_count = 0;
      index_count = 0;
      size_t current_offset = 0;
      size_t shared_offset = 0;
      size_t next_offset = 0;
      size_t index_offset = 0;
      const Ogre::MeshPtr mesh = pEntity->getMesh();
      unsigned short j = mesh->getNumSubMeshes();

      for(unsigned short i = 0; i < mesh->getNumSubMeshes(); ++i)
      {
        Ogre::SubMesh* submesh = mesh->getSubMesh(i);

        if(submesh->useSharedVertices)
        {
          if(!added_shared)
          {
            mVertexCount += (unsigned int)mesh->sharedVertexData->vertexCount;
            added_shared = true;
          }
        }
        else
        {
          mVertexCount += (unsigned int)submesh->vertexData->vertexCount;
        }

        mIndexCount += (unsigned int)submesh->indexData->indexCount;
      }

      vertices = new Ogre::Vector3[mVertexCount];
      indices = new unsigned long[mIndexCount];
      NxVec3* mMeshVertices = new NxVec3[mVertexCount];
      NxU32* mMeshFaces = new NxU32[mIndexCount];
      NxMaterialIndex* mMaterials = new NxMaterialIndex[mIndexCount];
      NxMaterialIndex currentMaterialIndex = 0;
      added_shared = false;

      for(unsigned short i = 0; i < mesh->getNumSubMeshes(); i++)
      {
        Ogre::SubMesh* submesh = mesh->getSubMesh(i);
        Ogre::VertexData* vertex_data = submesh->useSharedVertices ? mesh->sharedVertexData : submesh->vertexData;

        // Material Aliases..

        //currentMaterialIndex = _scene->findMaterialIndex(submesh->getMaterialName());

        if((!submesh->useSharedVertices) || (submesh->useSharedVertices && !added_shared))
        {
          if(submesh->useSharedVertices)
          {
            added_shared = true;
            shared_offset = current_offset;
          }

          const Ogre::VertexElement* posElem = vertex_data->vertexDeclaration->findElementBySemantic(Ogre::VES_POSITION);
          Ogre::HardwareVertexBufferSharedPtr vbuf = vertex_data->vertexBufferBinding->getBuffer(posElem->getSource());
          unsigned char* vertex = static_cast<unsigned char*>(vbuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
          Ogre::Real* pReal;

          for(size_t j = 0; j < vertex_data->vertexCount; ++j, vertex += vbuf->getVertexSize())
          {
            posElem->baseVertexPointerToElement(vertex, &pReal);
            mMeshVertices[current_offset + j] = NxVec3(pReal[0] * vScale.x , pReal[1] * vScale.y, pReal[2] * vScale.z);
            //mMeshVertices[current_offset + j] = NxVec3(pReal[0] *  1.0f ,pReal[1] * 1.0f, pReal[2] * 1.0f);
          }

          vbuf->unlock();
          next_offset += vertex_data->vertexCount;
        }

        Ogre::IndexData* index_data = submesh->indexData;
        size_t numTris = index_data->indexCount / 3;
        Ogre::HardwareIndexBufferSharedPtr ibuf = index_data->indexBuffer;
        bool use32bitindexes = (ibuf->getType() == Ogre::HardwareIndexBuffer::IT_32BIT);

        if(use32bitindexes)
        {
          unsigned int*  pInt = static_cast<unsigned int*>(ibuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
          size_t offset = (submesh->useSharedVertices) ? shared_offset : current_offset;

          for(size_t k = 0; k < numTris * 3; ++k)
          {
            mMeshFaces[index_offset] = pInt[k] + static_cast<unsigned int>(offset);
            mMaterials[index_offset++] = currentMaterialIndex;
          }
        }
        else
        {
          unsigned short* pShort = reinterpret_cast<unsigned short*>(ibuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
          size_t offset = (submesh->useSharedVertices) ? shared_offset : current_offset;

          for(size_t k = 0; k < numTris * 3; ++k)
          {
            mMeshFaces[index_offset] = static_cast<unsigned int>(pShort[k]) + static_cast<unsigned int>(offset);
            mMaterials[index_offset++] = currentMaterialIndex;
          }
        }

        ibuf->unlock();
        current_offset = next_offset;
      }

      NxTriangleMeshDesc mTriangleMeshDescription;
      mTriangleMeshDescription.numVertices            = mVertexCount;
      mTriangleMeshDescription.numTriangles            = mIndexCount / 3;
      mTriangleMeshDescription.pointStrideBytes         = sizeof(NxVec3);
      mTriangleMeshDescription.triangleStrideBytes      = 3 * sizeof(NxU32);
      Ogre::LogManager::getSingleton().logMessage("5");
      //if (smooth)
      mTriangleMeshDescription.points                  = mMeshVertices;
      //else
      //   mTriangleMeshDescription.points                  = mMeshVertices_Smoothed;
      mTriangleMeshDescription.flags                  = NX_MF_HARDWARE_MESH;
      mTriangleMeshDescription.triangles               = mMeshFaces;
      mTriangleMeshDescription.materialIndexStride      = sizeof(NxMaterialIndex);
      mTriangleMeshDescription.materialIndices         = mMaterials;
      MemoryWriteBuffer buf;

      if(0)
      {
        Ogre::String fn;

        if(1)
        {
          fn = mesh->getName();

          if(Ogre::StringUtil::endsWith(fn, ".mesh"))
          {
            fn = fn.substr(0, fn.length() - 5) + ".nxs";
          }
          else
          {
            fn  += ".nxs";
          }
        }
        else
        {
          fn = "cooked.nxs";
        }

        NxInitCooking();

        if(NxCookTriangleMesh(mTriangleMeshDescription, UserStream(fn.c_str(), false)))
        {
          lNxTriangleMeshDesc->meshData = PxSDK->createTriangleMesh(UserStream(fn.c_str(), true));
        }
        else
        {
          hasWarning = true;
#ifdef _DEBUG
          //error::getSingleton().Throw("Failed meshShape cooking for Mesh '" + mesh->getName() + "'", error::WARNING, mesh->getName(), "meshShape");
#endif
        }
      }
      else
      {
        NxInitCooking();

        if(NxCookTriangleMesh(mTriangleMeshDescription, buf))
        {
          lNxTriangleMeshDesc->meshData = PxSDK->createTriangleMesh(MemoryReadBuffer(buf.data));
        }
        else
        {
          hasWarning = true;
#ifdef _DEBUG
#endif
        }
      }

      NxCloseCooking();
      delete []vertices;
      delete []indices;
      delete []mMeshVertices;
      delete []mMeshFaces;
      delete []mMaterials;
      /*

        NxActorDesc ActorDesc;

        ActorDesc.shapes.pushBack(lNxTriangleMeshDesc);
        gScene->createActor(ActorDesc);*/
      return lNxTriangleMeshDesc;
    }

    NxActor* PhysxManager::createFullMeshShapeFromModel2(Entity *pEntity, Vector3 vScale)
    {
      String sMeshName = pEntity->getMesh()->getName();
      NxTriangleMeshShapeDesc *pOutDesc;
      pOutDesc = new NxTriangleMeshShapeDesc();
      pOutDesc->setToDefault();
      unsigned int   mVertexCount = 0;
      unsigned int   mIndexCount  = 0;
      size_t         vertex_count;
      Ogre::Vector3*   vertices;
      size_t         index_count;
      unsigned long*   indices;
      bool added_shared = false;
      bool hasWarning = false;
      vertex_count = 0;
      index_count = 0;
      size_t current_offset = 0;
      size_t shared_offset = 0;
      size_t next_offset = 0;
      size_t index_offset = 0;
      //Ogre::Root::getSingleton().getMeshManager()->load(sMeshName, "General");
      const Ogre::MeshPtr mesh = pEntity->getMesh();//reinterpret_cast<Ogre::Mesh *>(Ogre::Root::getSingleton().getMeshManager()->getByName(sMeshName).get() );
      ushort j = mesh->getNumSubMeshes();

      for(unsigned short i = 0; i < mesh->getNumSubMeshes(); ++i)
      {
        Ogre::SubMesh* submesh = mesh->getSubMesh(i);

        if(submesh->useSharedVertices)
        {
          if(!added_shared)
          {
            mVertexCount += (uint)mesh->sharedVertexData->vertexCount;
            added_shared = true;
          }
        }
        else
        {
          mVertexCount += (uint)submesh->vertexData->vertexCount;
        }

        mIndexCount += (uint)submesh->indexData->indexCount;
      }

      vertices = new Ogre::Vector3[mVertexCount];
      indices = new unsigned long[mIndexCount];
      NxVec3* mMeshVertices = new NxVec3[mVertexCount];
      NxU32* mMeshFaces = new NxU32[mIndexCount];
      NxMaterialIndex* mMaterials = new NxMaterialIndex[mIndexCount];
      NxMaterialIndex currentMaterialIndex = 0;
      added_shared = false;

      for(unsigned short i = 0; i < mesh->getNumSubMeshes(); ++i)
      {
        Ogre::SubMesh* submesh = mesh->getSubMesh(i);
        Ogre::VertexData* vertex_data = submesh->useSharedVertices ? mesh->sharedVertexData : submesh->vertexData;

        // Material Aliases..

        //currentMaterialIndex = _scene->findMaterialIndex(submesh->getMaterialName());

        if((!submesh->useSharedVertices) || (submesh->useSharedVertices && !added_shared))
        {
          if(submesh->useSharedVertices)
          {
            added_shared = true;
            shared_offset = current_offset;
          }

          const Ogre::VertexElement* posElem = vertex_data->vertexDeclaration->findElementBySemantic(Ogre::VES_POSITION);
          Ogre::HardwareVertexBufferSharedPtr vbuf = vertex_data->vertexBufferBinding->getBuffer(posElem->getSource());
          unsigned char* vertex = static_cast<unsigned char*>(vbuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
          Ogre::Real* pReal;

          for(size_t j = 0; j < vertex_data->vertexCount; ++j, vertex += vbuf->getVertexSize())
          {
            posElem->baseVertexPointerToElement(vertex, &pReal);
            mMeshVertices[current_offset + j] = NxVec3(pReal[0] * vScale.x , pReal[1] * vScale.y, pReal[2] * vScale.z);
            //mMeshVertices[current_offset + j] = NxVec3(pReal[0] *  1.0f ,pReal[1] * 1.0f, pReal[2] * 1.0f);
          }

          vbuf->unlock();
          next_offset += vertex_data->vertexCount;
        }

        Ogre::IndexData* index_data = submesh->indexData;
        size_t numTris = index_data->indexCount / 3;
        Ogre::HardwareIndexBufferSharedPtr ibuf = index_data->indexBuffer;
        bool use32bitindexes = (ibuf->getType() == Ogre::HardwareIndexBuffer::IT_32BIT);

        if(use32bitindexes)
        {
          unsigned int*  pInt = static_cast<unsigned int*>(ibuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
          size_t offset = (submesh->useSharedVertices) ? shared_offset : current_offset;

          for(size_t k = 0; k < numTris * 3; ++k)
          {
            mMeshFaces[index_offset] = pInt[k] + static_cast<unsigned int>(offset);
            mMaterials[++index_offset] = currentMaterialIndex;
          }
        }
        else
        {
          unsigned short* pShort = reinterpret_cast<unsigned short*>(ibuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
          size_t offset = (submesh->useSharedVertices) ? shared_offset : current_offset;

          for(size_t k = 0; k < numTris * 3; ++k)
          {
            mMeshFaces[index_offset] = static_cast<unsigned int>(pShort[k]) + static_cast<unsigned int>(offset);
            mMaterials[++index_offset] = currentMaterialIndex;
          }
        }

        ibuf->unlock();
        current_offset = next_offset;
      }

      NxTriangleMeshDesc mTriangleMeshDescription;
      mTriangleMeshDescription.numVertices            = mVertexCount;
      mTriangleMeshDescription.numTriangles            = mIndexCount / 3;
      mTriangleMeshDescription.pointStrideBytes         = sizeof(NxVec3);
      mTriangleMeshDescription.triangleStrideBytes      = 3 * sizeof(NxU32);
      Ogre::LogManager::getSingleton().logMessage("5");
      //if (smooth)
      mTriangleMeshDescription.points                  = mMeshVertices;
      //else
      //   mTriangleMeshDescription.points                  = mMeshVertices_Smoothed;
      mTriangleMeshDescription.flags                  = NX_MF_HARDWARE_MESH;
      mTriangleMeshDescription.triangles               = mMeshFaces;
      mTriangleMeshDescription.materialIndexStride      = sizeof(NxMaterialIndex);
      mTriangleMeshDescription.materialIndices         = mMaterials;
      MemoryWriteBuffer buf;

      if(1)
      {
        Ogre::String fn;

        if(1)
        {
          fn = mesh->getName();

          if(Ogre::StringUtil::endsWith(fn, ".mesh"))
          {
            fn = fn.substr(0, fn.length() - 5) + ".nxs";
          }
          else
          {
            fn  += ".nxs";
          }
        }
        else
        {
          fn = "cooked.nxs";
        }

        NxInitCooking();

        if(NxCookTriangleMesh(mTriangleMeshDescription, UserStream(fn.c_str(), false)))
        {
          pOutDesc->meshData = PxSDK->createTriangleMesh(UserStream(fn.c_str(), true));
        }
        else
        {
          hasWarning = true;
#ifdef _DEBUG
          //error::getSingleton().Throw("Failed meshShape cooking for Mesh '" + mesh->getName() + "'", error::WARNING, mesh->getName(), "meshShape");
#endif
        }
      }
      else
      {
        if(NxCookTriangleMesh(mTriangleMeshDescription, buf))
        {
          pOutDesc->meshData = PxSDK->createTriangleMesh(MemoryReadBuffer(buf.data));
        }
        else
        {
          hasWarning = true;
#ifdef _DEBUG
#endif
        }
      }

      NxCloseCooking();
      delete []vertices;
      delete []indices;
      delete []mMeshVertices;
      delete []mMeshFaces;
      delete []mMaterials;
      NxActor     *actor;
      NxActorDesc   actorDesc;
      actorDesc.shapes.push_back(pOutDesc);
      actor = PxScene->createActor(actorDesc);
      return actor;
    }

    void PhysxManager::addBox(String name, Vector3 pos)
    {
      Atom      newAtom;
      NxActorDesc   actorDesc;
      NxBodyDesc    bodyDesc;
      NxBoxShapeDesc  boxDesc;
      Entity *ent = mSceneMgr->createEntity(name, "crate.mesh");
      ent->setMaterialName("Crate");
      newAtom.node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
      newAtom.node->attachObject(ent);
      newAtom.node->scale(50, 50, 50);
      newAtom.node->setPosition(pos);
      boxDesc.dimensions.set(toNx(ent->getBoundingBox().getHalfSize() * 50));
      boxDesc.group     = 2;
      boxDesc.mass      = 1000;
      boxDesc.materialIndex = defaultMaterial->getMaterialIndex();
      actorDesc.shapes.push_back(&boxDesc);
      actorDesc.body = &bodyDesc;
      actorDesc.density = 1000;
      actorDesc.globalPose.t = toNx(pos);
      //    actorDesc.group = 2;
      newAtom.actor = PxScene->createActor(actorDesc);
      //    newAtom.actor->setMass(250);
      newAtom.name = name;
      atoms.push_back(newAtom);
    };

    SceneNode* PhysxManager::getNode(String name)
    {
      for(unsigned int i = 0; i < atoms.size(); ++i)
        if(name == atoms[i].name)
        {
          return atoms[i].node;
        }
        else
        {
          return NULL;
        }

      return NULL;
    };

    NxActor* PhysxManager::getActor(String name)
    {
      for(unsigned int i = 0; i < atoms.size(); ++i)
        if(name == atoms[i].name)
        {
          return atoms[i].actor;
        }
        else
        {
          return NULL;
        }

      return NULL;
    };

    NxTriangleMeshShapeDesc CookeMesh(Entity *e, SceneNode *n)
    {
      LogManager::getSingletonPtr()->logMessage("01");
      //создаём Convex тело
      unsigned int  mVertexCount = 0;
      unsigned int  mIndexCount  = 0;
      size_t      vertex_count;
      Ogre::Vector3*  vertices;
      size_t      index_count;
      unsigned long*  indices;
      bool added_shared = false;
      vertex_count = 0;
      index_count = 0;
      size_t current_offset = 0;
      size_t shared_offset = 0;
      size_t next_offset = 0;
      size_t index_offset = 0;

      for(unsigned short i = 0; i < e->getMesh()->getNumSubMeshes(); ++i)
      {
        Ogre::SubMesh* submesh = e->getMesh()->getSubMesh(i);

        if(submesh->useSharedVertices)
        {
          if(!added_shared)
          {
            mVertexCount += e->getMesh()->sharedVertexData->vertexCount;
            added_shared = true;
          }
        }
        else
        {
          mVertexCount += submesh->vertexData->vertexCount;
        }

        mIndexCount += submesh->indexData->indexCount;
      }

      vertices = new Ogre::Vector3[mVertexCount];
      indices = new unsigned long[mIndexCount];
      NxVec3* mMeshVertices = new NxVec3[mVertexCount];
      NxU32* mMeshFaces = new NxU32[mIndexCount];
      NxMaterialIndex* mMaterials = new NxMaterialIndex[mIndexCount];
      NxMaterialIndex currentMaterialIndex = 0;
      added_shared = false;

      for(unsigned short i = 0; i < e->getMesh()->getNumSubMeshes(); ++i)
      {
        Ogre::SubMesh* submesh = e->getMesh()->getSubMesh(i);
        Ogre::VertexData* vertex_data = submesh->useSharedVertices ? e->getMesh()->sharedVertexData : submesh->vertexData;

        if((!submesh->useSharedVertices) || (submesh->useSharedVertices && !added_shared))
        {
          if(submesh->useSharedVertices)
          {
            added_shared = true;
            shared_offset = current_offset;
          }

          const Ogre::VertexElement* posElem = vertex_data->vertexDeclaration->findElementBySemantic(Ogre::VES_POSITION);
          Ogre::HardwareVertexBufferSharedPtr vbuf = vertex_data->vertexBufferBinding->getBuffer(posElem->getSource());
          unsigned char* vertex = static_cast<unsigned char*>(vbuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
          float* pReal;

          for(size_t j = 0; j < vertex_data->vertexCount; ++j, vertex += vbuf->getVertexSize())
          {
            posElem->baseVertexPointerToElement(vertex, &pReal);
            mMeshVertices[current_offset + j] = NxVec3(pReal[0], pReal[1], pReal[2]);
          }

          vbuf->unlock();
          next_offset += vertex_data->vertexCount;
        }

        Ogre::IndexData* index_data = submesh->indexData;
        size_t numTris = index_data->indexCount / 3;
        Ogre::HardwareIndexBufferSharedPtr ibuf = index_data->indexBuffer;
        bool use32bitindexes = (ibuf->getType() == Ogre::HardwareIndexBuffer::IT_32BIT);

        if(use32bitindexes)
        {
          unsigned int*  pInt = static_cast<unsigned int*>(ibuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
          size_t offset = (submesh->useSharedVertices) ? shared_offset : current_offset;

          for(size_t k = 0; k < numTris * 3; ++k)
          {
            mMeshFaces[index_offset] = pInt[k] + static_cast<unsigned int>(offset);
            mMaterials[++index_offset] = currentMaterialIndex;
          }
        }
        else
        {
          unsigned short* pShort = reinterpret_cast<unsigned short*>(ibuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
          size_t offset = (submesh->useSharedVertices) ? shared_offset : current_offset;

          for(size_t k = 0; k < numTris * 3; ++k)
          {
            mMeshFaces[index_offset] = static_cast<unsigned int>(pShort[k]) + static_cast<unsigned int>(offset);
            mMaterials[++index_offset] = currentMaterialIndex;
          }
        }

        ibuf->unlock();
        current_offset = next_offset;
      }

      NxTriangleMeshDesc triDesc;
      triDesc.numVertices     = mVertexCount;
      triDesc.points        = mMeshVertices;
      triDesc.pointStrideBytes  = sizeof(NxVec3);
      triDesc.numTriangles    = mIndexCount / 3;
      triDesc.triangleStrideBytes = 3 * sizeof(NxU32);
      triDesc.triangles     = mMeshFaces;
      triDesc.flags       = NX_CF_COMPUTE_CONVEX;
      NxTriangleMeshShapeDesc mShapeDesc;
      NxInitCooking();
      // Cooking from memory
      MemoryWriteBuffer buf;
      bool status = NxCookTriangleMesh(triDesc, buf);
      mShapeDesc.meshData = PxSDK->createTriangleMesh(MemoryReadBuffer(buf.data));
      mShapeDesc.group = GROUP_COLLIDABLE_NON_PUSHABLE;
      LogManager::getSingleton().logMessage("shape create");
      return mShapeDesc;
    };

    void addAtom(String name, NxActor *actor, SceneNode *node = 0)
    {
      //создаём новый атом
      Atom      newAtom;
      //присваиваем актор
      newAtom.actor = actor;

      //присваиваем имя и нод
      if(node != 0)
      {
        newAtom.node = node;
      }
      else
      {
        newAtom.node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
      }

      newAtom.name = name;
      //ложим атом в вектор
      atoms.push_back(newAtom);
    };

    //методы для конвертирования переменных из огра в физикс и обратно
    Vector3 PhysxManager::toOgre(const NxVec3 &v)
    {
      return Vector3(v.x, v.y, v.z);
    }
    NxVec3 PhysxManager::toNx(const Vector3 &v)
    {
      return NxVec3(v.x, v.y, v.z);
    }
    Quaternion PhysxManager::toOgre(const NxQuat &q)
    {
      return Quaternion(q.w, q.x, q.y, q.z);
    }
    NxQuat PhysxManager::toNx(Quaternion& q, bool _normalise = true)
    {
      if(_normalise)
      {
        q.normalise();
      }

      NxQuat a;
      a.x = q.x;
      a.y = q.y;
      a.z = q.z;
      a.w = q.w;
      return a;
    }

    PhysxManager::~PhysxManager(void)
    {
      //    PxScene->getPhysicsSDK().releaseHeightField(*heightField);
      //    delete [] heightFieldDesc->samples;
      //    delete heightFieldDesc;
      //    if (mTerrainActor)
      //      PxScene->releaseActor(*mTerrainActor);
      /*    if (!atoms.empty())
          {
            for ( std::vector<Atom>::iterator iter = atoms.begin(); iter != atoms.end(); ++iter)
            {
              Atom* obj = &(*iter);
              atoms.erase( iter);
              delete obj;
            }
          }*/
    }
};
