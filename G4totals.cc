// read in total cross sections data from G4NEUTRONHPDATA
// to compare to NT_XS


#include <iostream>
#include <iomanip>


#include "G4RunManager.hh"
#include "G4MTRunManager.hh"
#include "G4NistManager.hh"

#include "MinimalUserInitialization.hh"


#include "G4HadronElasticProcess.hh"
#include "G4Neutron.hh"
#include "G4ProcessManager.hh"

#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"

#include "G4Material.hh"
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

    runManager->SetUserInitialization(new NeutronPhysicsList);
    runManager->SetUserInitialization(new MinimalDetector);
    runManager->SetUserInitialization(new MinimalInitialization);


    const G4ParticleDefinition *theNeutron = G4Neutron::Definition();

    // get material (Si)
    G4NistManager *nist = G4NistManager::Instance();
    G4Material *material = nist->FindOrBuildMaterial("G4_Si");

    //G4ParticleHPElastic *elastic = new G4ParticleHPElastic;
    //elastic->BuildPhysicsTable(*theNeutron);
    
    runManager->Initialize();

    G4ProcessManager *theMan = theNeutron->GetProcessManager();
    G4ProcessVector *procList = theMan->GetProcessList();

    G4HadronElasticProcess *elasticProc = static_cast<G4HadronElasticProcess*>((*procList)[1]);

    G4CrossSectionDataStore *elasticDataStore = elasticProc->GetCrossSectionDataStore();

    G4DynamicParticle *dynamicNeutron = new G4DynamicParticle(theNeutron, G4ThreeVector(0.,0.,1.), 0.);

    //dynamicNeutron->SetKineticEnergy(50.*keV);
    //G4cout << "Cross section for 50 keV neutron with Si = " << cm*elasticDataStore->GetCrossSection(dynamicNeutron, material) << " cm^-1" << G4endl;

    std::ofstream outputStream("output1.txt");

    outputStream << std::setprecision(17);


    std::vector<G4double> energies = {10.*keV, 20.*keV, 10.*keV, 20.*keV, 80.*eV, 2.*MeV};

    for (auto e : energies) {
        dynamicNeutron->SetKineticEnergy(e);
        outputStream << e << " " << cm*elasticDataStore->GetCrossSection(dynamicNeutron, material) << G4endl;
    }

    outputStream.close();   


    delete runManager;
    
    return 0;
}


