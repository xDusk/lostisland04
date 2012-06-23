/*
 * This source file is a modified part of SkyX (Copyright (C) 2009 Xavier
 * Verguín González <xavierverguin@hotmail.com>, <xavyiy@gmail.com>).
 *
 * Actually, this modified source file is part of SonSilentSea.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor Boston, MA 02110-1301,  USA
 */
/*
    Authors:
    - Verguín González, Xavier
    - Cercos Pita, Jose Luis
*/

/*
Releses notes:
- Added CopyOptimal3DCellArraysData method, that allow to copy segments of the cells matrix.
- Added PerformOptimalCalculations method, that allow preforms calculations of segments of the cells matrix.
- Added mUpdateTimeTmp var, that store the mUpdateTime to setup in the next cycle*.
- Changed SetUpdateTime, that now store the new info in mUpdateTimeTmp, waiting a cycle end.
- Changed Update, that now performs calculations of segments of the cells matrix in all iterations, but only sets the texture at semi-cycles.

* cycle = set of frames needed to update the two textures
*/

#include <VClouds/DataManager.h>

#include <VClouds/VClouds.h>
#include <VClouds/Ellipsoid.h>

namespace SkyX
{
  namespace VClouds
  {
    DataManager::DataManager(VClouds *vc)
      : mVClouds(vc)
      , mCellsCurrent(0)
      , mCellsTmp(0)
      , mFFRandom(0)
      , mNx(0), mNy(0), mNz(0)
      , mCurrentTransition(0)
      , mUpdateTime(10.0f)
      , mUpdateTimeTmp(10.0f)
      , mMaxNumberOfClouds(250)
      , mVolTexToUpdate(true)
      , mCreated(false)
    {
      for(int k = 0; k < 2; k++)
      {
        mVolTextures[k].setNull();
      }
    }

    DataManager::~DataManager()
    {
      remove();
    }

    void DataManager::remove()
    {
      if(!mCreated)
      {
        return;
      }

      for(int k = 0; k < 2; k++)
      {
        Ogre::TextureManager::getSingleton().remove(mVolTextures[k]->getName());
        mVolTextures[k].setNull();
      }

      _delete3DCellArray(mCellsCurrent, mNx, mNy);
      _delete3DCellArray(mCellsTmp, mNx, mNy);
      delete mFFRandom;
      mNx = mNy = mNz = 0;
      mCreated = false;
    }

    static int mX = 0;
    void DataManager::update(const Ogre::Real &timeSinceLastFrame)
    {
      int ObjNx;

      // 1st part of cycle (VOL_TEX0)
      if(mVolTexToUpdate)
      {
        // Increase the time
        mCurrentTransition += timeSinceLastFrame;
        // Calc the position that correspond for this time
        ObjNx = Ogre::Math::Clamp((int)ceil(mNx * mCurrentTransition / mUpdateTime), 0, mNx);
        // Perfomr calculation of the segment
        PerformOptimalCalculations(mX, 0, 0, ObjNx, mNy, mNz);
        // Update actual cell position
        mX = ObjNx;

        if(mCurrentTransition > mUpdateTime)
        {
          // Update the texture
          _updateVolTextureData(mCellsCurrent, VOL_TEX0, mNx, mNy, mNz);
          // Reinit the cell position
          mX = 0;
          // Prepare 2nd part of cycle
          mCurrentTransition = mUpdateTime;
          mVolTexToUpdate = !mVolTexToUpdate;
        }
      }
      // 2nd part of cycle (VOL_TEX2)
      else
      {
        // Decrease the time
        mCurrentTransition -= timeSinceLastFrame;
        // Calc the position that correspond for this time
        ObjNx = Ogre::Math::Clamp((int)ceil(mNx * (1.0 - mCurrentTransition / mUpdateTime)), 0, mNx);
        // Perfomr calculation of the segment
        PerformOptimalCalculations(mX, 0, 0, ObjNx, mNy, mNz);
        // Update actual cell position
        mX = ObjNx;

        if(mCurrentTransition < 0)    // Then, we need to update the texture
        {
          // Update the texture
          _updateVolTextureData(mCellsCurrent, VOL_TEX1, mNx, mNy, mNz);
          // Reinit the cell position
          mX = 0;
          // Prepare 2nd part of cycle
          mCurrentTransition = 0;
          mVolTexToUpdate = !mVolTexToUpdate;
          // With ended cycle, we could update mUpdateTime
          mUpdateTime = mUpdateTimeTmp;
        }
      }
    }

    void DataManager::create(const int &nx, const int &ny, const int &nz)
    {
      remove();
      mNx = nx;
      mNy = ny;
      mNz = nz;
      mFFRandom = new FastFakeRandom(1024, 0, 1);
      _initData(nx, ny, nz);

      for(int k = 0; k < 2; k++)
      {
        _createVolTexture(static_cast<VolTextureId>(k), nx, ny, nz);
      }

      /*  setWheater(1, 1);

        _performCalculations(nx, ny, nz);
        _updateVolTextureData(mCellsCurrent, VOL_TEX0, mNx, mNy, mNz);

        _performCalculations(nx, ny, nz);
        _updateVolTextureData(mCellsCurrent, VOL_TEX1, mNx, mNy, mNz); */
      mCreated = true;
    }

    void DataManager::forceToUpdateData()
    {
      if(mVolTexToUpdate)
      {
        _performCalculations(mNx, mNy, mNz);
        _updateVolTextureData(mCellsCurrent, VOL_TEX0, mNx, mNy, mNz);
        mCurrentTransition = mUpdateTime;
        mVolTexToUpdate = !mVolTexToUpdate;
      }
      else
      {
        _performCalculations(mNx, mNy, mNz);
        _updateVolTextureData(mCellsCurrent, VOL_TEX1, mNx, mNy, mNz);
        mCurrentTransition = 0;
        mVolTexToUpdate = !mVolTexToUpdate;
      }
    }

    void DataManager::_initData(const int& nx, const int& ny, const int& nz)
    {
      mCellsCurrent = _create3DCellArray(nx, ny, nz);
      mCellsTmp     = _create3DCellArray(nx, ny, nz);
    }

    DataManager::Cell *** DataManager::_create3DCellArray(const int& nx, const int& ny, const int& nz, const bool& init)
    {
      Cell ***c = new Cell** [nx];
      int u, v, w;

      for(u = 0; u < nx; u++)
      {
        c[u] = new Cell* [ny];

        for(v = 0; v < ny; v++)
        {
          c[u][v] = new Cell[nz];
        }
      }

      if(!init)
      {
        return c;
      }

      for(u = 0; u < nx; u++)
      {
        for(v = 0; v < ny; v++)
        {
          for(w = 0; w < nz; w++)
          {
            c[u][v][w].act = false;
            c[u][v][w].cld = false;
            c[u][v][w].hum = false;
            c[u][v][w].pact = 0;
            c[u][v][w].pext = 1;
            c[u][v][w].phum = 0;
            c[u][v][w].dens = 0.0f;
            c[u][v][w].light = 0.0f;
          }
        }
      }

      return c;
    }

    void DataManager::_delete3DCellArray(Cell ***c, const int& nx, const int& ny)
    {
      int u, v;

      for(u = 0; u < nx; u++)
      {
        for(v = 0; v < ny; v++)
        {
          delete [] c[u][v];
        }

        delete [] c[u];
      }

      delete [] c;
    }

    void DataManager::CopyOptimal3DCellArraysData(Cell ***orig, Cell ***dest, const int& x, const int& y, const int& z, const int& nx, const int& ny, const int& nz)
    {
      int u, v, w;

      for(u = x; u < nx; u++)
      {
        for(v = y; v < ny; v++)
        {
          for(w = z; w < nz; w++)
          {
            dest[u][v][w].act = orig[u][v][w].act;
            dest[u][v][w].cld = orig[u][v][w].cld;
            dest[u][v][w].hum = orig[u][v][w].hum;
            dest[u][v][w].pact = orig[u][v][w].pact;
            dest[u][v][w].pext = orig[u][v][w].pext;
            dest[u][v][w].phum = orig[u][v][w].phum;
            dest[u][v][w].dens = orig[u][v][w].dens;
            dest[u][v][w].light = orig[u][v][w].light;
          }
        }
      }
    }
    void DataManager::_copy3DCellArraysData(Cell ***orvar, Cell ***dest, const int& nx, const int& ny, const int& nz)
    {
      int u, v, w;

      for(u = 0; u < nx; u++)
      {
        for(v = 0; v < ny; v++)
        {
          for(w = 0; w < nz; w++)
          {
            dest[u][v][w].act = orvar[u][v][w].act;
            dest[u][v][w].cld = orvar[u][v][w].cld;
            dest[u][v][w].hum = orvar[u][v][w].hum;
            dest[u][v][w].pact = orvar[u][v][w].pact;
            dest[u][v][w].pext = orvar[u][v][w].pext;
            dest[u][v][w].phum = orvar[u][v][w].phum;
            dest[u][v][w].dens = orvar[u][v][w].dens;
            dest[u][v][w].light = orvar[u][v][w].light;
          }
        }
      }
    }

    void DataManager::setWheater(const float& Humidity, const float& AverageCloudsSize, const int& NumberOfForcedUpdates)
    {
      int numberofclouds = static_cast<int>(Humidity * mMaxNumberOfClouds);
      Ogre::Vector3 maxcloudsize = AverageCloudsSize * Ogre::Vector3(mNx / 14, mNy / 14, static_cast<int>(static_cast<float>(mNz) / 2.75));
      // Update old clouds with new parameters
      Ogre::Vector3 currentdimensions;
      std::vector<Ellipsoid*>::const_iterator mEllipsoidsIt;

      for(mEllipsoidsIt = mEllipsoids.begin(); mEllipsoidsIt != mEllipsoids.end(); mEllipsoidsIt++)
      {
        currentdimensions = (*mEllipsoidsIt)->getDimensions();

        if(currentdimensions.x / maxcloudsize.x < 0.5 || currentdimensions.x / maxcloudsize.x > 2)
        {
          currentdimensions.x = maxcloudsize.x + Ogre::Math::RangeRandom(-0.2, 0.2) * maxcloudsize.x;
        }

        if(currentdimensions.y / maxcloudsize.y < 0.5 || currentdimensions.y / maxcloudsize.y > 2)
        {
          currentdimensions.y = maxcloudsize.y + Ogre::Math::RangeRandom(-0.2, 0.2) * maxcloudsize.y;
        }

        if(currentdimensions.z / maxcloudsize.z < 0.5 || currentdimensions.x / maxcloudsize.z > 2)
        {
          currentdimensions.z = maxcloudsize.z + Ogre::Math::RangeRandom(-0.2, 0.2) * maxcloudsize.z;
        }
      }

      // Remove some clouds if needed
      while(static_cast<unsigned int>(numberofclouds) < mEllipsoids.size())
      {
        mEllipsoids.pop_back();
      }

      // Add new clouds if needed
      Ogre::Vector3 newdim;
      Ogre::Vector3 maxdim;
      Ogre::Vector3 center;

      while(static_cast<unsigned int>(numberofclouds) > mEllipsoids.size())
      {
        newdim = Ogre::Vector3((int)(maxcloudsize.x * Ogre::Math::RangeRandom(0.5, 2.0)),
                               (int)(maxcloudsize.y * Ogre::Math::RangeRandom(0.5, 2.0)),
                               (int)(maxcloudsize.z * Ogre::Math::RangeRandom(0.8, 1.2)));
        maxdim = Ogre::Vector3(mNx, mNy, mNz);
        center = Ogre::Vector3((int)Ogre::Math::RangeRandom(newdim.x + 3.0, mNx - newdim.x - 3.0),
                               (int)Ogre::Math::RangeRandom(newdim.y + 3.0, mNy - newdim.y - 3.0),
                               (int)Ogre::Math::RangeRandom(newdim.z + 3.0, mNz - newdim.z - 3.0));
        addEllipsoid(new Ellipsoid(newdim.x,   newdim.y,   newdim.z,
                                   maxdim.x,   maxdim.y,   maxdim.z,
                                   center.x,   center.y,   center.z,
                                   Ogre::Math::RangeRandom(1.0f, 5.0f)), false);
      }

      _updateProbabilities(mCellsCurrent, mNx, mNy, mNz);

      for(int k = 0; k < NumberOfForcedUpdates; k++)
      {
        forceToUpdateData();
      }
    }

    void DataManager::addEllipsoid(Ellipsoid *e, const bool& UpdateProbabilities)
    {
      mEllipsoids.push_back(e);

      if(UpdateProbabilities)
      {
        e->updateProbabilities(mCellsCurrent, mNx, mNy, mNz);
      }
    }

    void DataManager::_clearProbabilities(Cell*** c, const int& nx, const int& ny, const int& nz)
    {
      int u, v, w;

      for(u = 0; u < nx; u++)
      {
        for(v = 0; v < ny; v++)
        {
          for(w = 0; w < nz; w++)
          {
            c[u][v][w].pact = 0;
            c[u][v][w].pext = 1;
            c[u][v][w].phum = 0;
          }
        }
      }
    }

    void DataManager::_updateProbabilities(Cell*** c, const int& nx, const int& ny, const int& nz, const bool& ClearProbabilities)
    {
      if(ClearProbabilities)
      {
        _clearProbabilities(c, nx, ny, nz);
      }

      std::vector<Ellipsoid*>::const_iterator mEllipsoidsIt;

      for(mEllipsoidsIt = mEllipsoids.begin(); mEllipsoidsIt != mEllipsoids.end(); mEllipsoidsIt++)
      {
        (*mEllipsoidsIt)->updateProbabilities(c, nx, ny, nz);
      }
    }

    const Ogre::Real DataManager::_getLightAbsorcionAt(Cell*** c, const int& nx, const int& ny, const int& nz, const int& x, const int& y, const int& z, const Ogre::Vector3& d, const float& att) const
    {
      Ogre::Real step = 1, factor = 1;
      Ogre::Vector3 pos = Ogre::Vector3(x, y, z);
      bool outOfBounds = false;
      int u, v, w, uu, vv,
          current_iteration = 0, max_iterations = 8;

      while(!outOfBounds)
      {
        w = (int)pos.z;

        if((int)pos.z >= nz || (int)pos.z < 0 || factor <= 0 || current_iteration >= max_iterations)
        {
          outOfBounds = true;
        }
        else
        {
          u = (int)pos.x;
          v = (int)pos.y;
          uu = u;

          while(uu >= nx)
          {
            uu -= nx;
          }

          while(uu < 0)
          {
            uu += nx;
          }

          vv = v;

          while(vv >= ny)
          {
            vv -= ny;
          }

          while(vv < 0)
          {
            vv += ny;
          }

          factor -= c[uu][vv][w].dens * att * (1.0 - static_cast<float>(current_iteration) / max_iterations);
          pos += step * (-d);
          current_iteration++;
        }
      }

      return Ogre::Math::Clamp<Ogre::Real>(factor, 0, 1);
    }

    void DataManager::PerformOptimalCalculations(const int& x, const int& y, const int& z, const int& nx, const int& ny, const int& nz)
    {
      int u, v, w;
      // Light scattering
      Ogre::Vector3 SunDir = Ogre::Vector3(mVClouds->getSunDirection().x, mVClouds->getSunDirection().z, mVClouds->getSunDirection().y);

      // Loop over the particles (selected cells)
      for(u = x; u < nx; u++)
      {
        for(v = y; v < ny; v++)
        {
          for(w = z; w < nz; w++)
          {
            // --- First step ------------------------------------------------------------------
            // ti+1/2                       ti
            mCellsCurrent[u][v][w].hum =  mCellsTmp[u][v][w].hum && !mCellsTmp[u][v][w].act;
            mCellsCurrent[u][v][w].cld =  mCellsTmp[u][v][w].cld ||  mCellsTmp[u][v][w].act;
            mCellsCurrent[u][v][w].act = !mCellsTmp[u][v][w].act &&  mCellsTmp[u][v][w].hum && _fact(mCellsTmp, mNx, mNy, mNz, u, v, w);
            // ----------------------------------------------------------------- First step  ---
            // Transfer the info to temp cells
            mCellsTmp[u][v][w].hum = mCellsCurrent[u][v][w].hum;
            mCellsTmp[u][v][w].cld = mCellsCurrent[u][v][w].cld;
            mCellsTmp[u][v][w].act = mCellsCurrent[u][v][w].act;
            // --- Second step -----------------------------------------------------------------
            // ti+1                       ti+1/2
            mCellsCurrent[u][v][w].hum = mCellsTmp[u][v][w].hum || (mFFRandom->get() < mCellsTmp[u][v][w].phum);
            mCellsCurrent[u][v][w].cld = mCellsTmp[u][v][w].cld && (mFFRandom->get() > mCellsTmp[u][v][w].pext);
            mCellsCurrent[u][v][w].act = mCellsTmp[u][v][w].act || (mFFRandom->get() < mCellsTmp[u][v][w].pact);
            // ---------------------------------------------------------------- Second step  ---
            // --- Continuity ec ---------------------------------------------------------------
            mCellsCurrent[u][v][w].dens = _getDensityAt(mCellsCurrent, mNx, mNy, mNz, u, v, w, 1);
            // -------------------------------------------------------------- Continuity ec  ---
            // --- Scattering ec ---------------------------------------------------------------
            mCellsCurrent[u][v][w].light = _getLightAbsorcionAt(mCellsCurrent, mNx, mNy, mNz, u, v, w, SunDir, 0.15f);
            // -------------------------------------------------------------- Scattering ec  ---
          }
        }
      }

      // Backup the data
      CopyOptimal3DCellArraysData(mCellsCurrent, mCellsTmp, x, y, z, nx, ny, nz);
    }

    void DataManager::_performCalculations(const int& nx, const int& ny, const int& nz)
    {
      // First step
      int u, v, w;

      for(u = 0; u < nx; u++)
      {
        for(v = 0; v < ny; v++)
        {
          for(w = 0; w < nz; w++)
          {
            // ti+1                       ti
            mCellsCurrent[u][v][w].hum =  mCellsTmp[u][v][w].hum && !mCellsTmp[u][v][w].act;
            mCellsCurrent[u][v][w].cld =  mCellsTmp[u][v][w].cld ||  mCellsTmp[u][v][w].act;
            mCellsCurrent[u][v][w].act = !mCellsTmp[u][v][w].act &&  mCellsTmp[u][v][w].hum && _fact(mCellsTmp, nx, ny, nz, u, v, w);
          }
        }
      }

      // Second step
      _copy3DCellArraysData(mCellsCurrent, mCellsTmp, nx, ny, nz);

      for(u = 0; u < nx; u++)
      {
        for(v = 0; v < ny; v++)
        {
          for(w = 0; w < nz; w++)
          {
            // ti+1                       ti
            mCellsCurrent[u][v][w].hum = mCellsTmp[u][v][w].hum || (mFFRandom->get() < mCellsTmp[u][v][w].phum);
            mCellsCurrent[u][v][w].cld = mCellsTmp[u][v][w].cld && (mFFRandom->get() > mCellsTmp[u][v][w].pext);
            mCellsCurrent[u][v][w].act = mCellsTmp[u][v][w].act || (mFFRandom->get() < mCellsTmp[u][v][w].pact);
          }
        }
      }

      // Final steps

      // Continous density
      for(u = 0; u < nx; u++)
      {
        for(v = 0; v < ny; v++)
        {
          for(w = 0; w < nz; w++)
          {
            mCellsCurrent[u][v][w].dens = _getDensityAt(mCellsCurrent, nx, ny, nz, u, v, w, 1);
            //mCellsCurrent[u][v][w].dens = _getDensityAt(mCellsCurrent,u,v,w);
          }
        }
      }

      // Light scattering
      Ogre::Vector3 SunDir = Ogre::Vector3(mVClouds->getSunDirection().x, mVClouds->getSunDirection().z, mVClouds->getSunDirection().y);

      for(u = 0; u < nx; u++)
      {
        for(v = 0; v < ny; v++)
        {
          for(w = 0; w < nz; w++)
          {
            mCellsCurrent[u][v][w].light = _getLightAbsorcionAt(mCellsCurrent, mNx, mNy, mNz, u, v, w, SunDir, 0.15f);
          }
        }
      }

      _copy3DCellArraysData(mCellsCurrent, mCellsTmp, nx, ny, nz);
    }

    const bool DataManager::_fact(Cell ***c, const int& nx, const int& ny, const int& nz, const int& x, const int& y, const int& z) const
    {
      bool i1m, j1m, k1m,
           i1r, j1r, k1r,
           i2r, i2m, j2r, j2m, k2r;
      i1m = ((x + 1) >= nx) ? c[0][y][z].act : c[x + 1][y][z].act;
      j1m = ((y + 1) >= ny) ? c[x][0][z].act : c[x][y + 1][z].act;
      k1m = ((z + 1) >= nz) ? false : c[x][y][z + 1].act;
      i1r = ((x - 1) < 0) ? c[nx - 1][y][z].act : c[x - 1][y][z].act;
      j1r = ((y - 1) < 0) ? c[x][ny - 1][z].act : c[x][y - 1][z].act;
      k1r = ((z - 1) < 0) ? false : c[x][y][z - 1].act;
      i2r = ((x - 2) < 0) ? c[nx - 2][y][z].act : c[x - 2][y][z].act;
      j2r = ((y - 2) < 0) ? c[x][ny - 2][z].act : c[x][y - 2][z].act;
      k2r = ((z - 2) < 0) ? false : c[x][y][z - 2].act;
      i2m = ((x + 2) >= nx) ? c[1][y][z].act : c[x + 2][y][z].act;
      j2m = ((y + 2) >= ny) ? c[x][1][z].act : c[x][y + 2][z].act;
      return i1m || j1m || k1m  || i1r || j1r || k1r || i2r || i2m || j2r || j2m || k2r;
    }

    const float DataManager::_getDensityAt(Cell ***c, const int& nx, const int& ny, const int& nz, const int& x, const int& y, const int& z, const int& r) const
    {
      int zr = ((z - r) < 0) ? 0 : z - r,
          zm = ((z + r) >= nz) ? nz : z + r,
          u, uu, v, vv, w,
          clouds = 0, div = 0;

      for(u = x - r; u <= x + r; u++)
      {
        for(v = y - r; v <= y + r; v++)
        {
          for(w = zr; w < zm; w++)
          {
            // x/y Seamless!
            uu = u;

            while(uu >= nx)
            {
              uu -= nx;
            }

            while(uu < 0)
            {
              uu += nx;
            }

            vv = v;

            while(vv >= ny)
            {
              vv -= ny;
            }

            while(vv < 0)
            {
              vv += ny;
            }

            clouds += c[uu][vv][w].cld ? 1 : 0;
            div++;
          }
        }
      }

      return ((float)clouds) / div;
    }

    const float DataManager::_getDensityAt(Cell ***c, const int& x, const int& y, const int& z) const
    {
      return c[x][y][z].cld ? 1.0f : 0.0f;
    }

    void DataManager::_createVolTexture(const VolTextureId& TexId, const int& nx, const int& ny, const int& nz)
    {
      mVolTextures[static_cast<int>(TexId)]
      = Ogre::TextureManager::getSingleton().
        createManual("_SkyX_VolCloudsData" + Ogre::StringConverter::toString(TexId),
                     SKYX_RESOURCE_GROUP,
                     Ogre::TEX_TYPE_3D,
                     nx, ny, nz, 0,
                     Ogre::PF_BYTE_RGB);
      mVolTextures[static_cast<int>(TexId)]->load();
      static_cast<Ogre::MaterialPtr>(
        Ogre::MaterialManager::getSingleton().getByName("SkyX_VolClouds"))
      ->getTechnique(0)->getPass(0)->getTextureUnitState(static_cast<int>(TexId))
      ->setTextureName("_SkyX_VolCloudsData" + Ogre::StringConverter::toString(TexId), Ogre::TEX_TYPE_3D);
    }

    void DataManager::_updateVolTextureData(Cell ***c, const VolTextureId& TexId, const int& nx, const int& ny, const int& nz)
    {
      Ogre::HardwarePixelBufferSharedPtr buffer = mVolTextures[TexId]->getBuffer(0, 0);
      buffer->lock(Ogre::HardwareBuffer::HBL_DISCARD);
      const Ogre::PixelBox &pb = buffer->getCurrentLock();
      Ogre::uint16 *pbptr = static_cast<Ogre::uint16*>(pb.data);
      size_t x, y, z;

      for(z = pb.front; z < pb.back; z++)
      {
        for(y = pb.top; y < pb.bottom; y++)
        {
          for(x = pb.left; x < pb.right; x++)
          {
            Ogre::PixelUtil::packColour(c[x][y][z].dens, c[x][y][z].light, 0, 0, Ogre::PF_BYTE_RGB, &pbptr[x]);
          }

          pbptr += pb.rowPitch;
        }

        pbptr += pb.getSliceSkip();
      }

      buffer->unlock();
    }
  }
}
