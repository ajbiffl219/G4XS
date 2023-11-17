// estimate differential cross sections from Geant4 (probability distribution over final states) and write to file


#include <iostream>
#include <iomanip>

#include "G4RunManager.hh"
#include "G4MTRunManager.hh"
#include "G4NistManager.hh"

#include "MinimalUserInitialization.hh"

#include "G4HadronElasticProcess.hh"



int main(int argc, char **argv) {

#ifdef G4MULTITHREADED  
    G4MTRunManager * runManager = new G4MTRunManager(); 
    G4cout << runManager->GetNumberOfThreads() << " threads" << G4endl;
#else
    G4RunManager * runManager = new G4RunManager();
    G4cout << "sequential mode" << G4endl;
#endif

    G4String mat;
    if (argc == 2) {
        mat = argv[1];
        G4cout << "set material to " << mat << G4endl;
    } else {
        mat = "G4_H";
    }

    //runManager->SetVerboseLevel(0);
    //G4VModularPhysicsList *physList = new NeutronPhysicsList;
    //physList->SetVerboseLevel(0);
    //G4cout << "phys list verbosity = " << physList->GetVerboseLevel() << G4endl;
    runManager->SetUserInitialization(new NeutronPhysicsList);
    runManager->SetUserInitialization(new MinimalDetector(mat));
    runManager->SetUserInitialization(new MinimalInitialization);

    //G4HadronicProcessStore::Instance()->SetVerbose(0); // this line throws warnings

    G4NistManager *nist = G4NistManager::Instance();
    G4Material *material = nist->FindOrBuildMaterial(mat);


    const G4ParticleDefinition *theNeutron = G4Neutron::Definition();

    runManager->Initialize();

    G4ProcessManager *theMan = theNeutron->GetProcessManager();
    G4ProcessVector *procList = theMan->GetProcessList();
    G4HadronElasticProcess *elasticProc = static_cast<G4HadronElasticProcess*>((*procList)[1]);

    for (size_t p = 1; p < 5; ++p) {
        std::vector<G4HadronicInteraction*> interactionList = static_cast<G4HadronicProcess*>((*procList)[p])->GetHadronicInteractionList();

        G4cout << (*procList)[p]->GetProcessName() << " : " << interactionList.size() << " interactions" << G4endl;

        for (size_t i = 0; i < interactionList.size(); ++i) {
            
            G4cout << "Model " << interactionList[i]->GetModelName() << G4endl;

            interactionList[i]->ModelDescription(G4cout);
        }
    }

    G4cout << "done" << G4endl;

    delete runManager;

    return 0;
}
