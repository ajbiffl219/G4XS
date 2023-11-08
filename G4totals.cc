// read in total cross sections data from G4NEUTRONHPDATA
// to compare to NT_XS


#include <iostream>


#include "G4RunManager.hh"
#include "G4MTRunManager.hh"
#include "G4NistManager.hh"

#include "MinimalUserInitialization.hh"


#include "globals.hh"
#include "G4HadronElasticProcess.hh"
#include "G4ParticleHPElasticData.hh"
#include "G4ParticleHPElastic.hh"
#include "G4Neutron.hh"
#include "G4ProcessManager.hh"

#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"

#include "G4Material.hh"

#include "G4Box.hh"
#include "G4PVPlacement.hh"

#include "G4CrossSectionDataStore.hh"


//int main(int argc, char **argv) {
int main() {

#ifdef G4MULTITHREADED  
    G4MTRunManager * runManager = new G4MTRunManager(); 
    G4cout << runManager->GetNumberOfThreads() << " threads" << G4endl;
#else
    G4RunManager * runManager = new G4RunManager();
    G4cout << "sequential mode" << G4endl;
#endif

    G4VModularPhysicsList *phys = new NeutronPhysicsList();
    MinimalDetector *det = new MinimalDetector;

    runManager->SetUserInitialization(phys);
    runManager->SetUserInitialization(det);
    runManager->SetUserInitialization(new MinimalInitialization);


    const G4ParticleDefinition *theNeutron = G4Neutron::Definition();

    // get material (Si)
    G4NistManager *nist = G4NistManager::Instance();
    G4Material *material = nist->FindOrBuildMaterial("G4_Si");

    G4ParticleHPElastic *elastic = new G4ParticleHPElastic;
    elastic->BuildPhysicsTable(*theNeutron);
    G4cout << "3 Built G4ParticleHPElastic physics table" << G4endl;
    
    runManager->Initialize();
    G4cout << "2.3 initialized" << G4endl;

    G4ProcessManager *theMan = theNeutron->GetProcessManager();
    G4ProcessVector *procList = theMan->GetProcessList();

    G4HadronElasticProcess *elasticProc = static_cast<G4HadronElasticProcess*>((*procList)[1]);

    G4CrossSectionDataStore *elasticDataStore = elasticProc->GetCrossSectionDataStore();

    G4DynamicParticle *dynamicNeutron = new G4DynamicParticle(theNeutron, G4ThreeVector(0.,0.,1.), 0.);

    //G4double e = 50;

    //dynamicNeutron->SetKineticEnergy(e*keV);
    //G4cout << "Cross section for " << e << " keV neutron with Si = " << cm*elasticDataStore->GetCrossSection(dynamicNeutron, material) << " cm^-1" << G4endl;


    delete runManager;
    
    return 0;
}


