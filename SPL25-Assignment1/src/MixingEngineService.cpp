#include "MixingEngineService.h"
#include <iostream>
#include <memory>
#include <cmath>


/**
 * TODO: Implement MixingEngineService constructor
 */
MixingEngineService::MixingEngineService()
    : decks(), active_deck(1), auto_sync(false), bpm_tolerance(0)
{
    // Your implementation here
    decks[0] = nullptr;
    decks[1] = nullptr;
    active_deck = 0;
    auto_sync = false;
    bpm_tolerance = 0;
    std::cout << "[MixingEngineService] Initialized with 2 empty decks." << std::endl;
}

/**
 * TODO: Implement MixingEngineService destructor
 */
MixingEngineService::~MixingEngineService() {
    // Your implementation here
    std::cout << "[MixingEngineService] Cleaning up decks..." << std::endl;
    for(size_t i = 0; i<2; i++){
        if(decks[i]){
            delete decks[i];
        }
        decks[i] = nullptr;
    }
}


/**
 * TODO: Implement loadTrackToDeck method
 * @param track: Reference to the track to be loaded
 * @return: Index of the deck where track was loaded, or -1 on failure
 */
int MixingEngineService::loadTrackToDeck(const AudioTrack& track) {
    // Your implementation here
    std::cout << "\n=== Loading Track to Deck ===" << std::endl;
    PointerWrapper<AudioTrack> cloned = track.clone();
    if(!cloned){
        std::cout << "[ERROR] Track: " << track.get_title() << " failed to clone " << std::endl;
        return -1;
    }
    bool is_first_track = (decks[0] == nullptr && decks[1] == nullptr);
    size_t target = is_first_track ? 0 : 1-active_deck;
    std::cout << "[Deck Switch] Target deck: " << target << std::endl;
    if(decks[target] != nullptr){
        delete decks[target];
        decks[target] = nullptr;
    }
    cloned->load();
    cloned->analyze_beatgrid();
    if(auto_sync){
        if(decks[active_deck] !=nullptr){
            if(!can_mix_tracks(cloned)){
            sync_bpm(cloned);
            }
        }
        else {
            std::cout << "[Sync BPM] Cannot sync - one of the decks is empty." << std::endl;
        }
    }
    decks[target] = cloned.release();
    std::cout << "[Load Complete] '" << track.get_title() << "' is now loaded on deck " << target << std::endl;
    
    active_deck = target;
    std::cout << "[Active Deck] Switched to deck " << target << std::endl;
    displayDeckStatus();
    return target; // Placeholder
}

/**
 * @brief Display current deck status
 */
void MixingEngineService::displayDeckStatus() const {
    std::cout << "\n=== Deck Status ===\n";
    for (size_t i = 0; i < 2; ++i) {
        if (decks[i])
            std::cout << "Deck " << i << ": " << decks[i]->get_title() << "\n";
        else
            std::cout << "Deck " << i << ": [EMPTY]\n";
    }
    std::cout << "Active Deck: " << active_deck << "\n";
    std::cout << "===================\n";
}

/**
 * TODO: Implement can_mix_tracks method
 * 
 * Check if two tracks can be mixed based on BPM difference.
 * 
 * @param track: Track to check for mixing compatibility
 * @return: true if BPM difference <= tolerance, false otherwise
 */
bool MixingEngineService::can_mix_tracks(const PointerWrapper<AudioTrack>& track) const {
    // Your implementation here
    if(decks[active_deck] == nullptr || !track){
        return false;
    }
    int bpmdiff = std::abs(decks[active_deck]->get_bpm() - track->get_bpm());
    return bpmdiff <= bpm_tolerance; // Placeholder
}

/**
 * TODO: Implement sync_bpm method
 * @param track: Track to synchronize with active deck
 */
void MixingEngineService::sync_bpm(const PointerWrapper<AudioTrack>& track) const {
    // Your implementation here
    if(!track || decks[active_deck] == nullptr){
        return;
    }
    size_t og_bpm = track->get_bpm();
    size_t avg_bpm = (track->get_bpm() + decks[active_deck]->get_bpm()) / 2;
    track->set_bpm(avg_bpm);
    std::cout << "[Sync BPM] Syncing BPM from " << og_bpm << " to " << avg_bpm << std::endl;

}
