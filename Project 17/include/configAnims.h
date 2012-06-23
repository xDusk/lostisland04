#pragma once

#include "ActionManager.h"

using namespace Ogre;
enum BlendingTransition
{
  BlendSwitch,         // ������� ������������
  BlendWhileAnimating,   // ������������ ����������� ��� �������� �������� �� ������ � ������ ��� ������� � � ������ �������������
  BlendThenAnimate      // �������� �������� ������������ � ��������� ���� ����� ����������� ����� � ����������� ����
};
enum ActionTermination
{
  Limit,  //������������ ����������, ���������� � ������������ ���������� ���������, ���� ������� �� ����������,
  //���������� ���������� �������� � ������� ����� ������� mBreak=true
  Normal, //������� ����������, ����� �������� � ����� ������
  Final //������������� ����������, ����������� ����� ����������� ��������
};
enum Actions
{
  IDLE,     //������ �� ��������� �������
  WALKF,      //���� �����
  WALKB,      //�����
  WALKL,      //�����
  WALKR,      //������
  SLOWF,      //������� �����
  SLOWB,      //�����
  SLOWL,      //�����
  SLOWR,      //������
  RUNF,     //������ �����
  ONEHANDIDLE,  //���������� ������ � �����
  ONEHANDBLOCK, //���������� ����
  ONEHANDFORWARD, //���������� ���� �����
  GETHIT,
  ATTACK1,
  ATTACK2,
  ATTACK3,
};
struct configAnims
{
  Actions mActions; //��� ��������
  String mName;   //�������� ��������
  Real mSpeed;    //�������� ��������
  BlendingTransition mTransition;     //��� ����������
  ActionTermination mActionTermination; //��� ���������� ��������
  bool mBreak;    //��������� ����� �������� ���� true
  Real mDuration;   //����������������� ���������� � ��������
  bool mLooped;   //�������� ����������� ��� ���
  bool mSoundEnabled; //�������������� ������ ��� ���
};
