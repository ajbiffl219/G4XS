// estimate differential cross sections from Geant4 (probability distribution over final states) and write to file


#include <iostream>
#include <iomanip>

#include "G4RunManager.hh"
#include "G4MTRunManager.hh"
#include "G4NistManager.hh"

#include "MinimalUserInitialization.hh"

#include "G4HadronElasticProcess.hh"
#include "G4DynamicParticle.hh"
#include "G4HadProjectile.hh"
#include "G4HadFinalState.hh"
#include "G4Track.hh"
#include "Randomize.hh"

#include "G4SystemOfUnits.hh"

#include "G4ParticleHPManager.hh"


void writeEnergies(G4String fileName, G4double *data, G4int N) {
    std::ofstream os(fileName);
    os << std::setprecision(17);
    for (int i = 0; i < N; ++i) {
        os << i << " " << data[i]/eV << G4endl;
    }
    os.close();
}


int main(int argc, char **argv) {

    CLHEP::HepRandom::setTheSeed(time(NULL));

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
    } else {
        mat = "G4_Si";
    }

    G4cout << "set material to " << mat << G4endl;

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

    // initialize physics list
    runManager->Initialize();
    G4cout << "initialized" << G4endl;

    G4ProcessManager *theMan = theNeutron->GetProcessManager();
    G4ProcessVector *procList = theMan->GetProcessList();
    G4HadronElasticProcess *elasticProc = static_cast<G4HadronElasticProcess*>((*procList)[1]);
    //std::vector<G4HadronicInteraction*> interactionList = elasticProc->GetHadronicInteractionList();
    G4HadronicInteraction* elasticInteraction = elasticProc->GetHadronicInteractionList()[0];

    
    G4DynamicParticle *dynamicNeutron = new G4DynamicParticle(theNeutron, G4ThreeVector(0.,0.,1.), 0.);

    G4ThreeVector origin = G4ThreeVector(0.,0.,0.);
    
    G4StepPoint *thePoint = new G4StepPoint();
    thePoint->SetPosition(origin);
    thePoint->SetMaterial(material);

    G4Step *theStep = new G4Step();
    theStep->SetPreStepPoint(thePoint);
    
    
    G4Track *neutronTrack = new G4Track(dynamicNeutron, 0., origin);
    neutronTrack->SetStep(theStep);

    G4HadProjectile *projectile = new G4HadProjectile(*neutronTrack);
    G4cout << "created projectile" << G4endl;
    
    // get G4Nucleus
    G4Nucleus *materialNucleus = new G4Nucleus;
    //G4CrossSectionDataStore *elasticDataStore = elasticProc->GetCrossSectionDataStore();
    
    //elasticDataStore->ComputeCrossSection(dynamicNeutron, material); // need to compute cross sections in material before sampling Z/A
    //const G4Element *theElement = elasticDataStore->SampleZandA(dynamicNeutron, material, *materialNucleus);
    //elasticDataStore->SampleZandA(dynamicNeutron, material, *materialNucleus);
    //G4cout << "selected isotope" << G4endl;

    

    //for (size_t i = 0; i < interactionList.size(); ++i) {
    //    G4cout << "Model " << interactionList[i]->GetModelName() << G4endl;
    //    interactionList[i]->ModelDescription(G4cout);
    //}

    //G4cout << elasticInteraction->GetModelName() << G4endl;
    //elasticInteraction->SetVerboseLevel(10);

    //auto thing = projectile->GetMaterial();
    //if (!thing) { G4cout << "there's your problem" << G4endl;}

    G4HadFinalState *neutronFS;

    const G4int N = 500000;
    G4double finalEnergies[N];

    G4double Emin = 1.*keV, Emax = 10.*keV;
    G4int npoints = 10;

    G4String fileName;

    G4double initiale[] = {0.5*keV, 0.7*keV, 0.8*keV, 0.9*keV, 1.*keV, 1.1*keV, 1.15*keV};
    
    //for (G4int i = 0; i < npoints; ++i) {

    for (G4double &e : initiale) {

        //e = Emin + i*(Emax - Emin)/npoints;
        
        dynamicNeutron->SetKineticEnergy(e);
        neutronTrack->SetKineticEnergy(e);
        projectile->Initialise(*neutronTrack);

        for (G4int j = 0; j < N; ++j) {
            neutronFS = elasticInteraction->ApplyYourself(*projectile, *materialNucleus);
            finalEnergies[j] = neutronFS->GetEnergyChange();
        }
        fileName = "finalE" + std::to_string(e) + ".txt";
        G4cout << "Writing data to " << fileName << G4endl;
        writeEnergies(fileName, finalEnergies, N);
    }

    //G4HadFinalState *neutronFS = elasticInteraction->ApplyYourself(*projectile, *materialNucleus);

    //G4cout << neutronFS->GetEnergyChange() << G4endl; // from usage in G4HadronicProcess::FillResult(), GetEnergyChange() returns the final energy
    
    

    //G4cout << "rng: " << G4UniformRand() << G4endl;
    

    delete runManager;

    G4cout << "done" << G4endl;

    return 0;
}
