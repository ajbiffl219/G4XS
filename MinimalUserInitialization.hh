#ifndef MINIMALUSERINITIALIZATION_H
#define MINIMALUSERINITIALIZATION_H

#include "G4VModularPhysicsList.hh"
#include "G4VUserDetectorConstruction.hh"
#include "G4VUserActionInitialization.hh"
#include "G4VUserPrimaryGeneratorAction.hh"


class NeutronPhysicsList : public G4VModularPhysicsList {
public:
    NeutronPhysicsList();
    ~NeutronPhysicsList() {;}
    void ConstructProcess();
    void ConstructHadronics();
    void ConstructParticle();
};


class MinimalDetector : public G4VUserDetectorConstruction {
public:
    MinimalDetector(G4String mat = "G4_Galactic");
    ~MinimalDetector() {;}
    G4VPhysicalVolume* Construct();
protected:
    G4String material;
};



class MinimalInitialization : public G4VUserActionInitialization {
public:
    MinimalInitialization();
    ~MinimalInitialization() {;}
    void Build() const;
};


class MinimalPrimaryGenerator : public G4VUserPrimaryGeneratorAction {
public:
    MinimalPrimaryGenerator();
    ~MinimalPrimaryGenerator() {;}
    void GeneratePrimaries(G4Event *);
};


#endif