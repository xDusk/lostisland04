#include "header.h"

class Quest
{
  protected:
    String    Name;
    bool    Complete;
  public:
    Quest(String name)
    {
      Name = name;
      Complete = false;
    }

    void update()
    {
      //������� ������ �� ����� ��� id � �����������
    }
};

class QuestManager
{
  protected:
    std::vector<Quest> qList;
  public:
    void addQuest(String name);
    Quest* getQuest(String name)
    {
      //���� �� ������� � ����������
      return;
    }
};