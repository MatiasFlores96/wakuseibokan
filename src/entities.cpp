#include "entities.h"
#include "profiling.h"
#include "container.h"

// Add a new interface to an embodied object.
#include "units/BoxVehicle.h"
#include "units/Walrus.h"
#include "units/Manta.h"
#include "units/SimplifiedDynamicManta.h"
#include "units/Buggy.h"
#include "units/MultiBodyVehicle.h"
#include "units/Balaenidae.h"
#include "units/Beluga.h"
#include "units/AdvancedManta.h"
#include "units/Cephalopod.h"
#include "units/Medusa.h"
#include "units/Stingray.h"
#include "units/Otter.h"
#include "units/Turtle.h"
#include "units/CargoShip.h"

#include "terrain/Terrain.h"

#include "structures/Structure.h"
#include "structures/Runway.h"
#include "structures/Hangar.h"
#include "structures/Turret.h"
#include "structures/Warehouse.h"
#include "structures/Laserturret.h"
#include "structures/CommandCenter.h"
#include "structures/Artillery.h"
#include "structures/Launcher.h"
#include "structures/Dock.h"
#include "structures/Antenna.h"
#include "structures/Factory.h"
#include "structures/Radar.h"
#include "structures/Armory.h"
#include "structures/WindTurbine.h"

#include "actions/Gunshot.h"
#include "actions/Missile.h"
#include "actions/Bomb.h"
#include "actions/Debris.h"
#include "actions/LaserBeam.h"
#include "actions/LaserRay.h"
#include "actions/Torpedo.h"
#include "actions/Shell.h"
#include "actions/Missile.h"
#include "actions/AAM.h"


#include "actions/Explosion.h"

#include "weapons/CarrierArtillery.h"
#include "weapons/CarrierLauncher.h"
#include "weapons/CarrierTurret.h"

extern container<Vehicle*> entities;
extern std::vector<BoxIsland*> islands;

// @NOTE: Creates an entity from a tick record.  This replicates what "spawn" functions do in engine, but this is
//       used to create entities for replaying and networking.
void createEntity(TickRecord record,dSpaceID space, dWorldID world)
{
    if (record.typeId == EntityTypeId::TCarrierArtillery ||
        record.typeId == EntityTypeId::TCarrierLauncher ||
        record.typeId == EntityTypeId::TCarrierTurret)
    {
        Weapon *w = NULL;

        if (record.typeId == EntityTypeId::TCarrierArtillery)
            w = new CarrierArtillery(record.faction);
        else if (record.typeId == EntityTypeId::TCarrierLauncher)
            w = new CarrierLauncher(record.faction);
        else if (record.typeId == EntityTypeId::TCarrierTurret)
            w = new CarrierTurret(record.faction);

        w->init();
        w->embody(world, space);
        w->setPos(Vec3f(record.location.pos1,record.location.pos2, record.location.pos3));
        w->stop();

        entities.assign(record.id,w, w->getGeom());

    }


    if (record.typeId == EntityTypeId::TWheel)
    {
        Wheel *w = new Wheel(record.faction, 0.001, 30.0);
        w->init();
        w->embody(world, space);
        w->setPos(Vec3f(record.location.pos1,record.location.pos2, record.location.pos3));
        w->stop();

        entities.assign(record.id, w, w->getGeom());
    }

    if (record.typeId == EntityTypeId::TBalaenidae)
    {
        Balaenidae* b = new Balaenidae(record.faction);
        b->init();
        dSpaceID carrier_space = b->embody_in_space(world, space);
        b->setPos(Vec3f(record.location.pos1,record.location.pos2, record.location.pos3));
        b->stop();

        entities.assign(record.id, b, b->getGeom());


    }

    if (record.typeId == EntityTypeId::TBeluga)
    {
        Beluga* b = new Beluga(record.faction);
        b->init();
        dSpaceID carrier_space_beluga = b->embody_in_space(world, space);
        b->setPos(Vec3f(record.location.pos1,record.location.pos2, record.location.pos3));

        entities.assign(record.id, b, b->getGeom());

    }
    if (record.type == VehicleTypes::MANTA)
    {
        Manta *_manta1 = NULL;

        if (record.typeId == EntityTypeId::TMedusa)
            _manta1 = new Medusa(record.faction);
        else if (record.typeId == EntityTypeId::TStingray)
            _manta1 = new Stingray(record.faction);
        else if (record.typeId == EntityTypeId::TAdvancedManta)
            _manta1 = new AdvancedManta(record.faction);
        else if (record.typeId == EntityTypeId::TSimplifiedDynamicManta)
            _manta1 = new SimplifiedDynamicManta(record.faction);
        else if (record.typeId == EntityTypeId::TCephalopod)
            _manta1 = new Cephalopod(record.faction);

        assert( _manta1 != NULL || !"Unrecognized manta type on ledger.");

        _manta1->init();
        _manta1->embody(world, space);
        _manta1->setPos(Vec3f(record.location.pos1,record.location.pos2, record.location.pos3));
        _manta1->setStatus(FlyingStatus::FLYING);              // @FIXME, status should be stored.
        _manta1->setNameByNumber(record.number);
        _manta1->inert = true;

        entities.assign(record.id, _manta1, _manta1->getGeom());

    }


    if (record.type == VehicleTypes::WALRUS)
    {
        if (record.typeId == EntityTypeId::TCargoShip)
        {
            CargoShip *_cargoship = new CargoShip(record.faction);
            _cargoship->init();
            _cargoship->embody(world, space);
            _cargoship->setPos(Vec3f(record.location.pos1,record.location.pos2, record.location.pos3));
            _cargoship->setNameByNumber(record.number);
            _cargoship->setSignal(4);

            entities.assign(record.id, _cargoship, _cargoship->getGeom());

        }
        else if (record.typeId == EntityTypeId::TAdvancedWalrus)
        {
            Walrus *_walrus = new AdvancedWalrus(record.faction);
            _walrus->init();
            _walrus->embody(world, space);
            _walrus->setPos(Vec3f(record.location.pos1,record.location.pos2, record.location.pos3));
            _walrus->setNameByNumber(record.number);

            entities.assign(record.id, _walrus, _walrus->getGeom());

        }
        else if (record.typeId == EntityTypeId::TWalrus)
        {
            Walrus *_walrus = new Walrus(record.faction);
            _walrus->init();
            _walrus->embody(world, space);
            _walrus->setPos(Vec3f(record.location.pos1,record.location.pos2, record.location.pos3));
            _walrus->setNameByNumber(record.number);

            entities.assign(record.id, _walrus, _walrus->getGeom());

        }
        else if (record.typeId == EntityTypeId::TOtter)
        {
            Otter *_walrus = new Otter(record.faction);
            _walrus->init();
            dSpaceID car_space = _walrus->embody_in_space(world, space);
            _walrus->setPos(Vec3f(record.location.pos1,record.location.pos2, record.location.pos3));
            _walrus->setStatus(SailingStatus::SAILING);
            _walrus->setNameByNumber(record.number);

            entities.assign(record.id, _walrus, _walrus->getGeom());
        }
        else if (record.typeId == EntityTypeId::TTurtle)
        {
            Turtle *_walrus = new Turtle(record.faction);
            _walrus->init();
            dSpaceID car_space = _walrus->embody_in_space(world, space);
            _walrus->setPos(Vec3f(record.location.pos1,record.location.pos2, record.location.pos3));
            _walrus->setStatus(SailingStatus::SAILING);
            _walrus->setNameByNumber(record.number);

            entities.assign(record.id, _walrus, _walrus->getGeom());
        }
    }

    {
        Gunshot *action = NULL;


        if (record.typeId== EntityTypeId::TAAM)
                action = new AAM(record.faction);
        else if (record.typeId == EntityTypeId::TArtilleryAmmo)
                action = new ArtilleryAmmo();
        else if (record.typeId == EntityTypeId::TBomb)
                action = new Bomb(record.faction);
        else if (record.typeId == EntityTypeId::TDebris)
                action = new Debris();
        else if (record.typeId == EntityTypeId::TExplosion)
                action = new Explosion();
        else if (record.typeId == EntityTypeId::TGunshot)
                action = new Gunshot();
        else if (record.typeId == EntityTypeId::TLaserBeam)
            action = new LaserBeam();
        else if (record.typeId == EntityTypeId::TMissile)
            action = new Missile(record.faction);
        else if (record.typeId == EntityTypeId::TShell)
            action = new Shell();
        else if (record.typeId == EntityTypeId::TTorpedo)
            action = new Torpedo(record.faction);


        if (action)
        {

            // Need axis conversion.
            action->init();
            // @FIXME: Set the right origin.
            //action->setOrigin(entities[1]->getBodyID());
            action->embody(world,space);
            action->setPos(Vec3f(record.location.pos1,record.location.pos2, record.location.pos3));
            action->setVisible(true);

            entities.assign(record.id, action, action->getGeom());
        }
    }

    if (record.type == VehicleTypes::COLLISIONABLE ||
            record.type == VehicleTypes::CONTROL ||
            record.type == VehicleTypes::LANDINGABLE)
    {
        Structure *v = NULL;

        int typeofisland=0;

        switch (record.typeId) {
        case EntityTypeId::TArmory:
            v = new Armory(record.faction);
            break;
        case EntityTypeId::TArtillery:
            v = new Artillery(record.faction);
            break;
        case EntityTypeId::TCommandCenter:
            v = new CommandCenter(record.faction,typeofisland);
            break;
        case EntityTypeId::THangar:
            v = new Hangar(record.faction);
            break;
        case EntityTypeId::TWarehouse:
            v = new Warehouse(record.faction);
            break;
        case EntityTypeId::TRunway:
            v = new Runway(record.faction);
            break;
        case EntityTypeId::TLaserTurret:
            v = new LaserTurret(record.faction);
            break;
        case EntityTypeId::TTurret:
            v = new Turret(record.faction);
            break;
        case EntityTypeId::TLauncher:
            v = new Launcher(record.faction);
            break;
        case EntityTypeId::TFactory:
            v = new Factory(record.faction);
            break;
        case EntityTypeId::TDock:
            v = new Dock(record.faction);
            break;
        case EntityTypeId::TAntenna:
            v = new Antenna(record.faction);
            break;
        case EntityTypeId::TRadar:
            v = new Radar(record.faction);
            break;
        case EntityTypeId::TWindTurbine:
            v = new WindTurbine(record.faction);
            break;
        case EntityTypeId::TStructure:default:
            v = new Structure(record.faction);
            break;
        }

        // I need to recover the island that is associated with the structure.
        // The path to recreate the structure is the same that was used for creating originally the structure.

        BoxIsland *b = islands[record.islandId];

        //v->init();
        //v->embody(world,space); //islandspace, Island is missing
        //v->setPos(Vec3f(record.location.pos1,record.location.pos2, record.location.pos3));
        //v->rotate(record.orientation);
        //v->onIsland(this);

        islands[record.islandId]->attachStructure(v,record.id,record.location.pos1,record.location.pos2, record.location.pos3,record.orientation,world);
        entities.assign(record.id, v, v->getGeom());

    }
}



