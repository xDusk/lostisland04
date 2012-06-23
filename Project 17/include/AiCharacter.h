#pragma once

#include <Ogre.h>
#include "BaseObj.h"
#include "Player.h"
#include <vector>
#include "Items.h"


using namespace std;

class Cell
{
  public:
    Vector2 cGridPosition;  //позиция клетки в массиве клеток
    Vector2 cPosition;    //позиция центра клеткм в игровом мире
    Vector2 cParentPosition;//позиция в массиве родительской клетки
    int cValue;       //значение проходимости 1 - можно пройти. 0 - нельзя
    int cF, cG, cH;     //стоимость

    Cell(Vector2 pos, int val, int h = 0, int g = 0)
    {
      cPosition = pos;
      cValue = val;
      cH = h;
      cG = g;
      cF = cH + cG;
    }
    ~Cell() {}

    int getValue()
    {
      return cValue;
    }
    int getFPrice()
    {
      return cF;
    }
    int getGPrice()
    {
      return cG;
    }
    int getHPrice()
    {
      return cH;
    }
    Vector2 getParentPosition()
    {
      return cParentPosition;
    }
    Vector2 getPosition()
    {
      return cPosition;
    }
    Vector2 getGridPosition()
    {
      return cGridPosition;
    }

    void setValue(int val)
    {
      cValue = val;
    }

    void setFPrice(int f)
    {
      cF = f;
    }

    void setHPrice(int h)
    {
      cH = h;
      cF = cG + cH;
    }

    void setGPrice(int g)
    {
      cG = g;
      cF = cG + cH;
    }

    void setParentPosition(Vector2 pos)
    {
      cParentPosition = pos;
    }

    void setPosition(Vector2 pos)
    {
      cPosition = pos;
    }

    void setGridPosition(Vector2 pos)
    {
      cGridPosition = pos;
    }

};

class PathFinding
{
  private:
    int F;                // стоимость клетки F=H+G
    int G;                // стоимость перемещения из стартовой клетки в текущую
    int H;                // стоимость перемещения из текущей клетки в целевую
    Cell* grid[400][400];       // массив со всеми клетками
    std::vector<Cell*> openList;    // открытый список (клетки для просмотра)
    std::vector<Cell*> closeList;   // закрытый список (непросматриваемые клетки)
    std::vector<Vector2> findedPath;  // массив в ктором сохранятся точки конечного пути ...ааааа нада динамический
    Vector2 finishPosition;       // позиция стартовой и конечной клетки в массиве
    Vector2 startPosition;

  public:
    PathFinding() //создаём клетки
    {
      //тут нада грузить каким то образом сетку. щас вся открытая.
      int gx = -10000;
      int gy = -10000;

      for(int x = 0; x < 400; ++x)
        for(int y = 0; y < 400; ++y)
        {
          gx = 50 + (100 * x);
          gy = 50 + (100 * y);
          grid[x][y] = new Cell(Vector2(gx, gy), 1);
          grid[x][y]->setGridPosition(Vector2(x, y));

          if(mTerrainGroup->getHeightAtWorldPosition(Vector3(gx, 0, gy)) < 140)
          {
            grid[x][y]->cValue = 0;
          }
          else
          {
            grid[x][y]->cValue = 1;
          }
        }
    }

    std::vector<Vector2> FindPath(Vector2 from, Vector2 to)
    {
      LogManager::getSingletonPtr()->logMessage(__FUNCTION__);

      //ищем клетку старта и назначаем её стоимость 0
      for(int i = 0; i < 400; ++i)
        for(int j = 0; j < 400; ++j)
        {
          if((grid[i][j]->getPosition().x - from.x) < 50)
            if((grid[i][j]->getPosition().y - from.y) < 50)
            {
              startPosition = Vector2(i, j);
              grid[i][j]->setGPrice(0);
              grid[i][j]->setHPrice(((((int)finishPosition.x - grid[i][j]->getPosition().x) + ((int)finishPosition.y - grid[i][j]->getPosition().y)) * 10));
            }
        };

      LogManager::getSingletonPtr()->logMessage("startPosition");

      LogManager::getSingletonPtr()->logMessage(Ogre::StringConverter::toString(startPosition));

      //ищем клетку финиша
      for(int i = 0; i < 400; ++i)
        for(int j = 0; j < 400; ++j)
        {
          if((grid[i][j]->getPosition().x - to.x) < 50)
            if((grid[i][j]->getPosition().y - to.y) < 50)
            {
              finishPosition.x = i;
              finishPosition.y = j;
            }
        }

      LogManager::getSingletonPtr()->logMessage("finishPosition");
      LogManager::getSingletonPtr()->logMessage(Ogre::StringConverter::toString(finishPosition));
      //ложим стартовую точку в открытый список
      openList.push_back(grid[(int)startPosition.x][(int)startPosition.y]);
      openList[0]->setParentPosition(startPosition);
      //    iter++;//положили элемент мб нада увеличить?
      //создаём итератор открытого списка
      std::vector< Cell*>::iterator iter = openList.begin();
      //начало поиска
      Cell* tempF = openList[0]; //сюда будем писать клетку с минимальной стоимостью.

      //    #pragma omp parallel for num_threads(2)// распаралеливаем циклы мб заработает=)
      //цикл по точкам открытого списка. Для клетки с минимальной стоимостью проверяем соседние клетки на проходимость
      for(;;)
      {
        //      ищем клетку в открытом списке с минимальной стоимостью
        for(unsigned int j = 0; j < openList.size(); ++j)
        {
          if(openList[j]->getFPrice() < tempF->getFPrice())
          {
            tempF = openList[j];
          }
        }

        lookNear(tempF->getGridPosition(), iter);

        for(int k = 0; k < 400; ++k)
        {
          if(openList[k] != NULL)
            if(openList[k]->getGridPosition() == finishPosition) //проверяем на наличие конечной точки в открытом списке
            {
              finalizePath(openList[k]->getGridPosition());
              break;
            }
        }

        ++iter;

        if(openList.size() != 0)
        {
          break;
        }
      }

      //!!!!!!!!!!!!!!!!!!!!!!!!!! возвращать указатель на массив! не уверен что щас прально
      return findedPath; //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    }

    //в этом методе проверяем точки находящиеся вокруг заданой на проходимость. если проходима ложим в открытый список
    void lookNear(Vector2 pos, std::vector< Cell*>::iterator k)
    {
      LogManager::getSingletonPtr()->logMessage(__FUNCTION__);
      Cell* h = *k;
      //проверяем проходимость
      CheckCell(Vector2(pos.x - 1, pos.y - 1), 14, k);
      CheckCell(Vector2(pos.x,   pos.y - 1), 10, k);
      CheckCell(Vector2(pos.x + 1, pos.y - 1), 14, k);
      CheckCell(Vector2(pos.x + 1, pos.y),   10, k);
      CheckCell(Vector2(pos.x + 1, pos.y + 1), 14, k);
      CheckCell(Vector2(pos.x,   pos.y + 1), 10, k);
      CheckCell(Vector2(pos.x - 1, pos.y + 1), 14, k);
      CheckCell(Vector2(pos.x - 1, pos.y),   10, k);
      //ложим точку вокруг которой проверяли в закрытй список
      closeList.push_back(h);
      //удаляем провереную точку из открытого списка
      openList.erase(k);
    }

    void CheckCell(Vector2 pos, int dir, std::vector<Cell*>::iterator op)
    {
      LogManager::getSingletonPtr()->logMessage(__FUNCTION__);

      //проверяем нет ли этой точки уже в списке
      if(searchInOpen(pos) == false)
      {
        if(grid[(int)pos.x][(int)pos.y]->getValue() == 1)
        {
          LogManager::getSingletonPtr()->logMessage(Ogre::StringConverter::toString(pos));
          openList.push_back(grid[(int)pos.x][(int)pos.y]);                //ложим точку в список
          //op = openList.end();
          Cell *h = openList.back();
          h->setGPrice(grid[(int)pos.x][(int)pos.y]->getGPrice() + dir);  //увеличиваем для неё G(расстояние от старта) и H (до финиша)
          //расстояние до финиша щитаем с помощью вычисления Манхэттенского расстояния
          h->setHPrice(grid[(int)pos.x][(int)pos.y]->getHPrice() +
                       ((((int)finishPosition.x - h->getPosition().x) + ((int)finishPosition.y - h->getPosition().y)) * 10)); //расстояние до финиша щитаем с помощью вычисления Манхэттенского расстояния
          //проверяем G родителя и текущей клетки. если G родителя больше, меняем родителя
          //        int px = openList[n]->getParentPosition().x;
          //        int py = openList[n]->getParentPosition().y;
          //        if(grid[px][py]->getGPrice() > grid[(int)pos.x][(int)pos.y]->getGPrice())
          //          grid[px][py]->setParentPosition(grid[(int)pos.x][(int)pos.y]->getParentPosition());
        };
      };
    }

    bool searchInOpen(Vector2 pos)   //проверяем есть ли уже эта клетка в открытом списке
    {
      LogManager::getSingletonPtr()->logMessage(__FUNCTION__);
      bool ret = false;

      for(uint k = 0; k < openList.size(); ++k)
      {
        LogManager::getSingletonPtr()->logMessage("1");

        if(openList[k] != NULL)
          if(openList[k]->getGridPosition() == pos)
          {
            ret = true;
          }
      }

      return ret;
    }

    void finalizePath(Vector2 j)
    {
      LogManager::getSingletonPtr()->logMessage(__FUNCTION__);
      Vector2 gp = j;

      for(int i = 1;; ++i)
      {
        LogManager::getSingletonPtr()->logMessage(Ogre::StringConverter::toString(grid[(int)gp.x][(int)gp.y]->getPosition()));
        findedPath.push_back(grid[(int)gp.x][(int)gp.y]->getPosition());

        if(gp != grid[(int)gp.x][(int)gp.y]->getGridPosition())
        {
          gp = grid[(int)gp.x][(int)gp.y]->getParentPosition();
        }
        else
        {
          break;
        }
      }
    }

    //  viod updateGrid()
    //  {
    //передирать все динамические объекты, выщитывать к какой клетке они ближе всего и назначать её непроходимой
    //  }

    PathFinding::~PathFinding() {}
};

class AiCharacter : public CharacterObj
{
  private:
    bool            inWork;   //флаг действия
    Vector3     targetPos;  //куда щас идём
    PathFinding     *PF;    //создаём поисковик
    Vector3     To;     //куда идтить
    bool      isGo;   //щас идёт
    Real      hittime;  //время между ударами в секундах
    Real      ElapsedTime;//время кадра

    Entity      *HeadEntity;
    Entity      *LeftEyeEntity;
    Entity      *RightEyeEntity;
    Entity      *EarsEntity;
    SceneNode   *HeadNode;

    enum actions
    {
      moving,
      battle,
      doAction
    };

    std::vector<actions> actionsVec;


  public:

    Inventory *CharacterInv;      //инвентарь

    AiCharacter(String name, String meshName, PhysxManager *pxmgr, Player *player, SceneManager *sceneMgr, Vector3 pos)
    {
      inBattle = false;
      inWork = false;
      isGo = false;
      mName = name;
      mSceneMgr = sceneMgr;
      mPosition = pos;
      mOrientation = Quaternion(0, 1, 0, 0);
      mPxMgr = pxmgr;
      mPxSDK = pxmgr->PxSDK;
      mPxScene = pxmgr->PxScene;
      hittime = 3;
      //    CharacterInv = new Inventory( 10);
      //установка флагов
      mHealth = 500;
      mIsDead = false;
      mBlock  = false;
      inWork = false;
      //графика
      mMainNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(mName, mPosition);
      mEntity = mSceneMgr->createEntity(mName, "Guard.mesh");
      mEntity->setCastShadows(true);
      OffsetNode = mMainNode->createChildSceneNode(mName + "Child", Vector3(0, -2.25, 0));
      OffsetNode->attachObject(mEntity);
      OffsetNode->yaw(Degree(180));
      mMainNode->scale(10, 10, 10);
      HeadEntity = mSceneMgr->createEntity(mName + "Head", "head.mesh");
      HeadNode  = OffsetNode->createChildSceneNode(mName + "Head", Vector3(0, mEntity->getBoundingBox().getSize().y - 0.1, 0));
      HeadNode->attachObject(HeadEntity);
      LeftEyeEntity = mSceneMgr->createEntity(mName + "leftEye", "EyeLeftHuman.mesh");
      LeftEyeEntity->setMaterialName("humaneyel");
      RightEyeEntity = mSceneMgr->createEntity(mName + "rigthEye", "EyeRightHuman.mesh");
      RightEyeEntity->setMaterialName("humaneyer");
      HeadNode->attachObject(LeftEyeEntity);
      HeadNode->attachObject(RightEyeEntity);
      EarsEntity = mSceneMgr->createEntity(mName + "ears", "ears.mesh");
      HeadNode->attachObject(EarsEntity);
      //физика
      CreateCharacterController(mPosition);
      disp = NxVec3(0, -98.1f, 0);
      To = mMainNode->getPosition();
      //Добавляем анимации к скелету
      SkeletonManager::getSingleton().load("skeleton.skeleton", "General");
      addAnimationState("idle", "skeleton.skeleton");
      addAnimationState("walk_forward", "skeleton.skeleton");
      addAnimationState("walk_left", "skeleton.skeleton");
      addAnimationState("walk_right", "skeleton.skeleton");
      addAnimationState("walk_backward", "skeleton.skeleton");
      addAnimationState("walk_fast_forward", "skeleton.skeleton");
      //
      addAnimationState("one_hand_attack_left", "skeleton.skeleton");
      addAnimationState("handtohand_hook", "skeleton.skeleton");
      addAnimationState("one_hand_idle", "skeleton.skeleton");
      addAnimationState("one_hand_block", "skeleton.skeleton");
      addAnimationState("one_hand_forward", "skeleton.skeleton");
      //вектор со всеми анимациями
      std::vector<configAnims> ca;
      getConfigPlayer(ca);
      mActionManager = new ActionManager(mEntity, ca);
      mActionManager->init();
      this->getActionManager()->DoAction(IDLE);
      LogManager::getSingletonPtr()->logMessage(mName + "::Created");
    };

    String getType()
    {
      return "AiCharacter";
    }

    void createDoll()
    {
      //    mEntity->getSkeleton()->getBone( "Bip01 Head")->getPosition();
      //    doll->head->setGlobalPosition( )
    };

    Inventory* getInvebtory()
    {
      return CharacterInv;
    }

    virtual void Battle(CharacterObj *target)
    {
      LogManager::getSingletonPtr()->logMessage(mName + "::In Battle");
      hittime += ElapsedTime;
      //подойти к цели. и атаковать)
      Vector3 tmp, tmp2;
      tmp = mMainNode->getPosition();
      tmp.y = 0;
      tmp2 = target->getPosition();
      tmp2.y = 0;

      if(target->getHealth() > 0 && tmp.distance(tmp2) < 2000)
      {
        if(tmp.distance(tmp2) > 50.0)
        {
          // если  50<дистанция<1000 проверять экипировано ли дальнобойное оружие. если да атаковать им если нет идти ближе
          if(!isGo)
          {
            GoTo(mMainNode->getPosition(), target->getWorldPosition());
          }
        }
        else
        {
          StopWalk();

          if(hittime > 5)    //1 удар в 5 сек.
          {
            this->getActionManager()->DoAction(ATTACK1);
            this->Hit(target, 10);
            hittime = 0;
          }
        }
      }
      else
      {
        inBattle = false;
      }

      return;
    };

    virtual void OnTimeAction(Vector2 time)//в зависимости от времени выбирается действие на каждый промежуток несколько действий (рандом).
    {
      LogManager::getSingletonPtr()->logMessage(mName + "::OnTimeAction");

      if(time.x < 20.0f && time.x > 8.0f)
      {
        LogManager::getSingletonPtr()->logMessage(mName + "::OnTimeAction = шарится");
        To = Vector3(Math::RangeRandom(0, 0), 0, Math::RangeRandom(20000, 20000));
        GoTo(mMainNode->getPosition(), To);
        inWork = true;
      }
      else
      {
        LogManager::getSingletonPtr()->logMessage(mName + "::OnTimeAction = стоять");
        disp = NxVec3(0, -98, 0);
        mActionManager->DoAction(IDLE);
        inWork = true;
      }
    };

    void GoTo(Vector3 From, Vector3 To)
    {
      LogManager::getSingletonPtr()->logMessage(mName + "::GoTo");
      LogManager::getSingletonPtr()->logMessage("From");
      LogManager::getSingletonPtr()->logMessage(Ogre::StringConverter::toString(From));
      LogManager::getSingletonPtr()->logMessage("To");
      LogManager::getSingletonPtr()->logMessage(Ogre::StringConverter::toString(To));
      /*    targetPos = To; // на случай прерывания действия
          PF = new PathFinding();
          Vector2 F,T;
          std::vector<Vector2> Path;
          F.x = From.x;
          F.y = From.z;
          T.x = To.x;
          T.y = To.z;
          Path = PF->FindPath( F, T);
          delete PF;
          for (unsigned int i=0;i<Path.size();i++)
          {
          LogManager::getSingletonPtr()->logMessage(Ogre::StringConverter::toString(Path[i]));
          mMainNode->translate(Path[i].x, 0,Path[i].y); //тут мб путь будет в обратном порядке нада проверить.
          if(mMainNode->getPosition().x == T.x && mMainNode->getPosition().y == T.y )
            break;
          //if проверка на начало боя!!!!!!!!
          }*/
      isGo = true;
      horizontalDisp = mPxMgr->toNx(To - From);
      disp.x = horizontalDisp.x;
      disp.z = horizontalDisp.z;
      disp.normalize();
      disp.y = -98.1f;
      //disp *= ElapsedTime; почемуто слишком медленно
      mActionManager->DoAction(WALKF);
      return;
    };

    void StopWalk()
    {
      isGo = false;
      disp.x = 0;
      disp.z = 0;
      mActionManager->addTime(IDLE);
    }

    virtual void update(Real elapsedTime, Vector2 time)
    {
      ElapsedTime = elapsedTime;
      HeadNode->setPosition(mEntity->getSkeleton()->getBone("Bip01 Head")->_getDerivedPosition());
      HeadNode->setOrientation(mEntity->getSkeleton()->getBone("Bip01 Head")->_getDerivedOrientation());
      HeadNode->roll(Degree(-90));

      //обновление ии
      if(!mIsDead)
      {
        if(!inBattle)
        {
          if(time.x < 20.0f && time.x > 8.0f)
          {
            inWork = false;
          }

          Vector3 tmp, tmp2;
          tmp = mMainNode->getPosition();
          tmp.y = 0;
          tmp2 = To;
          tmp2.y = 0;

          if(tmp.distance(tmp2) < 50.0)
          {
            if(inWork != true) //проверка на наличие действия, если есть продолжаем если нету получаем новое в зависимости от времени
            {
              OnTimeAction(time);
            }
          }
          else
          {
            inWork = false;
          }

          //проверять не упёрся ли нпц в припятствие
          //collisionFlags сравнивать с //NXCC_COLLISION_SIDES  = (1<<0), //!< Character is colliding to the sides.
          //        if( collisionFlags<<1 && collisionFlags >> 0)
          //        {}
          //если упёрся запоминаем целевую позицию и получаем промежуточную
        }
        else
        {
          Battle(target);
        }
      }
      else
      {
        disp = NxVec3(0, -98.1, 0);
        createDoll();
      }

      //обновление контроллера и меша
      PxController->Move(disp, collisionFlags);
      PxControllerMgr->updateControllers();
      MeshDisp = mPxMgr->toOgre(disp) * (-1);
      MeshDisp.y = 0;
      mMainNode->setDirection(MeshDisp);
      mActionManager->addTime(elapsedTime);
    };
    ~AiCharacter(void)
    {
      delete mActionManager;
      mMainNode->detachAllObjects();
      mSceneMgr->destroyEntity(mEntity);
      mMainNode->removeAndDestroyAllChildren();
      mSceneMgr->destroySceneNode(mName);
    };
};
