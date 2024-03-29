/*
--------------------------------------------------------------------------------
This source file is part of SkyX.
Visit ---

Copyright (C) 2009 Xavier Vergu�n Gonz�lez <xavierverguin@hotmail.com>
                                           <xavyiy@gmail.com>

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
--------------------------------------------------------------------------------
*/

#include <VClouds/Ellipsoid.h>

namespace SkyX
{
  namespace VClouds
  {

    Ellipsoid::Ellipsoid(
      const int &a, const int &b, const int &c,
      const int &nx, const int &ny, const int &nz,
      const int &x, const int &y, const int &z,
      const Ogre::Real& Density)
      : mA(a), mB(b), mC(c)
      , mA2(Ogre::Math::Pow(a, 2)), mB2(Ogre::Math::Pow(b, 2)), mC2(Ogre::Math::Pow(c, 2))
      , mNx(nx), mNy(ny), mNz(nz)
      , mX(x), mY(y), mZ(z)
      , mDensity(Density)
    {
    }

    Ellipsoid::~Ellipsoid()
    {
    }

    const float Ellipsoid::_getLength(const int &x, const int &y, const int &z) const
    {
      //  x^2   y^2   z^2
      //  /   + /   + /    = 1  (Ellipsoid ecuation)
      //  a^2   b^2   c^2
      //
      //  maxradatdir = lambda (Xo, Yo, Zo) = lambda; where Xo, Yo and Zo are the components of the normaliced direction vector
      //
      //  => lambda^2 = 1 / ( EllipsoidEcuation...)
      //
      //  => lambda = sqrt (...) => maxradatdir = lambda
      Ogre::Vector3 Direction = Ogre::Vector3(x - mX, y - mY, z - mZ),
                    DirectionNormalized = Direction.normalisedCopy();
      Ogre::Real a = Ogre::Math::Pow(DirectionNormalized.x, 2) / mA2 +
                     Ogre::Math::Pow(DirectionNormalized.y, 2) / mB2 +
                     Ogre::Math::Pow(DirectionNormalized.z, 2) / mC2,
                     lambda = 1.0f / Ogre::Math::Sqrt(a);
      return Ogre::Math::Clamp<Ogre::Real>(Direction.length() / lambda, 0, 1);
    }

    const Ogre::Vector3 Ellipsoid::getProbabilities(const int& x, const int& y, const int& z) const
    {
      float density = Ogre::Math::Pow(1 - _getLength(x, y, z), 1.0f / mDensity);
      return Ogre::Vector3(density, 1 - density, density);
    }

    void Ellipsoid::updateProbabilities(DataManager::Cell ***c, const int &nx, const int &ny, const int &nz)
    {
      int u, v, w, uu, vv, ww;
      // Ogre::Vector3 prob;
      float length;

      for(u = mX - mA; u < mX + mA; u++)
      {
        for(v = mY - mB; v < mY + mB; v++)
        {
          for(w = mZ - mC; w < mZ + mC; w++)
          {
            //  prob = getProbabilities(u, v, w);
            // x/y Seamless!
            uu = (u < 0) ? (u + nx) : u;

            while(uu >= nx)
            {
              uu -= nx;
            }

            vv = (v < 0) ? (v + ny) : v;

            while(vv >= ny)
            {
              vv -= ny;
            }

            ww = (w < 0) ? (w + nz) : w;

            while(ww >= ny)
            {
              ww -= nz;
            }

            length = _getLength(uu, vv, ww);

            //  c[uu][vv][w].phum = Ogre::Math::Clamp<Ogre::Real>(c[uu][vv][w].phum+prob.x, 0, 1);
            //  c[uu][vv][w].pext = std::min<float>(prob.y, c[uu][vv][w].pext);
            //  c[uu][vv][w].pact = Ogre::Math::Clamp<Ogre::Real>(c[uu][vv][w].pact+prob.z, 0, 1);

            if(length < 1)
            {
              c[uu][vv][w].phum = 0.1f;
              c[uu][vv][w].pext = 0.5f;
              c[uu][vv][w].pact = 0.2f;
            }
          }
        }
      }
    }

    void Ellipsoid::move(const int& Ax, const int& Ay, const int& Az)
    {
      mX += Ax;
      mY += Ay;
      mZ += Az;
    }

    const bool Ellipsoid::isOutOfCells() const
    {
      if((mX + mA) >= mNx || (mX - mA) < 0 ||
         (mY + mB) >= mNy || (mY - mB) < 0 ||
         (mZ + mZ) >= mNz || (mZ - mZ) < 0)
      {
        return true;
      }

      return false;
    }

    void Ellipsoid::setDimensions(const Ogre::Vector3& Dimensions)
    {
      mA = Dimensions.x;
      mB = Dimensions.y;
      mC = Dimensions.z;
      mA2 = Ogre::Math::Pow(mA, 2);
      mB2 = Ogre::Math::Pow(mB, 2);
      mC2 = Ogre::Math::Pow(mC, 2);
    }

  }
}
