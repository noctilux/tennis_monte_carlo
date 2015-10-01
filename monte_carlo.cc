#include "iid_mc_model.h"
#include "adjusted_mc_model.h"
#include <map>
#include <algorithm>
#include <fstream>
#include <iostream>
#include "model_data.h"
#include "importance_model_data.h"
#include "importance_mc_model.h"

void test_model() {
  std::vector<ModelData> m = ModelData::ImportFromFile(
      "atp_points_predicted_slams_no_tournament_name.csv");

  std::ofstream o;

  o.open("iid_vs_non_iid_fixed_coin_slams.csv");

  o << "Player 1"
    << ","
    << "Player 2"
    << ","
    << "Match title"
    << ","
    << "p_win_iid"
    << ","
    << "p_win_dynamic"
    << ","
    << "p1_spw_iid"
    << ","
    << "p2_spw_iid"
    << ","
    << "average_sets"
    << ","
    << "average_sets_iid"
    << ","
    << "average_games"
    << ","
    << "average_games_iid" << std::endl;

  for (const ModelData &cur_match : m) {
    bool bo5 = true;

    const unsigned int kSimulations = 1E5;

    AdjustedMCModel adj(cur_match.p1(), cur_match.p2(), bo5, cur_match,
                        kSimulations);

    std::map<std::string, double> iid_probs;

    iid_probs[cur_match.p1()] =
        cur_match.ServeWinProbabilityIID(cur_match.p1());
    iid_probs[cur_match.p2()] =
        cur_match.ServeWinProbabilityIID(cur_match.p2());

    IIDMCModel iid_model(cur_match.p1(), cur_match.p2(), bo5, iid_probs,
                         kSimulations);

    const std::vector<Match> &matches = adj.matches();
    const std::vector<Match> &iid_matches = iid_model.matches();

    unsigned int i = std::count_if(
        matches.begin(), matches.end(),
        [cur_match](const Match &m) { return m.winner() == cur_match.p1(); });

    unsigned int iid_i = std::count_if(
        iid_matches.begin(), iid_matches.end(),
        [cur_match](const Match &m) { return m.winner() == cur_match.p1(); });

    unsigned int p1_served_first = std::count_if(matches.begin(), matches.end(),
                                                 [cur_match](const Match &m) {
      return m.server_at_start() == cur_match.p1();
    });

    unsigned int set_sum = 0;
    unsigned int set_sum_iid = 0;

    unsigned int game_sum = 0;
    unsigned int game_sum_iid = 0;

    for (unsigned int i = 0; i < kSimulations; ++i) {
      set_sum += matches[i].sets().size();
      set_sum_iid += iid_matches[i].sets().size();
      game_sum += matches[i].total_games();
      game_sum_iid += iid_matches[i].total_games();
    }

    double average_sets = set_sum / static_cast<double>(kSimulations);
    double average_sets_iid = set_sum_iid / static_cast<double>(kSimulations);

    double average_games = game_sum / static_cast<double>(kSimulations);
    double average_games_iid = game_sum_iid / static_cast<double>(kSimulations);

    double iid_win_prob = (iid_i / static_cast<double>(kSimulations));
    double non_iid_win_prob = (i / static_cast<double>(kSimulations));

    std::cout << "Match of " << cur_match.p1() << " against " << cur_match.p2()
              << std::endl;
    std::cout << "IID probability: " << iid_win_prob * 100 << "%" << std::endl;
    std::cout << "Non-IID probability: " << non_iid_win_prob * 100 << "%"
              << std::endl;
    std::cout << "IID sets: " << average_sets_iid
              << "; non-IID: " << average_sets << std::endl;
    std::cout << "IID games: " << average_games_iid
              << "; non-IID: " << average_games << std::endl;
    std::cout << cur_match.p1() << " served first "
              << p1_served_first / static_cast<double>(kSimulations) * 100
              << "% of the time." << std::endl;

    o << cur_match.p1() << "," << cur_match.p2() << ","
      << cur_match.match_title() << "," << iid_win_prob << ","
      << non_iid_win_prob << ","
      << cur_match.ServeWinProbabilityIID(cur_match.p1()) << ","
      << cur_match.ServeWinProbabilityIID(cur_match.p2()) << "," << average_sets
      << "," << average_sets_iid << "," << average_games << ","
      << average_games_iid << std::endl;
  }
  o.close();
}

void test_importance_model() {
  std::vector<ImportanceModelData> m =
      ImportanceModelData::ImportFromFile("atp_points_predicted_imp.csv");

  std::vector<ModelData> basic_matches =
      ModelData::ImportFromFile("atp_points_predicted.csv");

  std::ofstream o;

  o.open("iid_vs_non_iid_imp.csv");

  o << "Player 1"
    << ","
    << "Player 2"
    << ","
    << "Match title"
    << ","
    << "p_win_iid"
    << ","
    << "p_win_dynamic"
    << ","
    << "p_win_dynamic_basic"
    << ","
    << "p1_spw_iid"
    << ","
    << "p2_spw_iid"
    << ","
    << "average_sets"
    << ","
    << "average_sets_iid"
    << ","
    << "average_games"
    << ","
    << "average_games_iid" << std::endl;

  for (ImportanceModelData &cur_match : m) {
    bool bo5 = true;

    const unsigned int kSimulations = 1E5;

    ImportanceMCModel adj(cur_match.p1(), cur_match.p2(), bo5, cur_match,
                          kSimulations);

    // Find corresponding ModelData
    ModelData basic_m =
        *std::find_if(basic_matches.begin(), basic_matches.end(),
                      [cur_match](const ModelData &d) {
          return cur_match.p1() == d.p1() && cur_match.p2() == d.p2() &&
                 cur_match.match_title() == d.match_title();
        });

    AdjustedMCModel basic_adj(cur_match.p1(), cur_match.p2(), bo5, basic_m,
                              kSimulations);

    std::map<std::string, double> iid_probs;

    iid_probs[cur_match.p1()] =
        cur_match.ServeWinProbabilityIID(cur_match.p1());
    iid_probs[cur_match.p2()] =
        cur_match.ServeWinProbabilityIID(cur_match.p2());

    IIDMCModel iid_model(cur_match.p1(), cur_match.p2(), bo5, iid_probs,
                         kSimulations);

    const std::vector<Match> &matches = adj.matches();
    const std::vector<Match> &iid_matches = iid_model.matches();
    const std::vector<Match> &basic_matches = basic_adj.matches();

    unsigned int i = std::count_if(
        matches.begin(), matches.end(),
        [cur_match](const Match &m) { return m.winner() == cur_match.p1(); });

    unsigned int iid_i = std::count_if(
        iid_matches.begin(), iid_matches.end(),
        [cur_match](const Match &m) { return m.winner() == cur_match.p1(); });

    unsigned int basic_i = std::count_if(
        basic_matches.begin(), basic_matches.end(),
        [cur_match](const Match &m) { return m.winner() == cur_match.p1(); });

    unsigned int p1_served_first = std::count_if(matches.begin(), matches.end(),
                                                 [cur_match](const Match &m) {
      return m.server_at_start() == cur_match.p1();
    });

    unsigned int set_sum = 0;
    unsigned int set_sum_iid = 0;

    unsigned int game_sum = 0;
    unsigned int game_sum_iid = 0;

    for (unsigned int i = 0; i < kSimulations; ++i) {
      set_sum += matches[i].sets().size();
      set_sum_iid += iid_matches[i].sets().size();
      game_sum += matches[i].total_games();
      game_sum_iid += iid_matches[i].total_games();
    }

    double average_sets = set_sum / static_cast<double>(kSimulations);
    double average_sets_iid = set_sum_iid / static_cast<double>(kSimulations);

    double average_games = game_sum / static_cast<double>(kSimulations);
    double average_games_iid = game_sum_iid / static_cast<double>(kSimulations);

    double iid_win_prob = (iid_i / static_cast<double>(kSimulations));
    double non_iid_win_prob = (i / static_cast<double>(kSimulations));
    double basic_win_prob = (basic_i / static_cast<double>(kSimulations));

    std::cout << "Match of " << cur_match.p1() << " against " << cur_match.p2()
              << std::endl;
    std::cout << "IID probability: " << iid_win_prob * 100 << "%" << std::endl;
    std::cout << "Non-IID probability: " << non_iid_win_prob * 100 << "%"
              << std::endl;
    std::cout << "Basic non-iid: " << basic_win_prob * 100 << "%" << std::endl;
    std::cout << "IID sets: " << average_sets_iid
              << "; non-IID: " << average_sets << std::endl;
    std::cout << "IID games: " << average_games_iid
              << "; non-IID: " << average_games << std::endl;
    std::cout << cur_match.p1() << " served first "
              << p1_served_first / static_cast<double>(kSimulations) * 100
              << "% of the time." << std::endl;

    std::cout << ImportanceModelData::total_missing_points_ << " out of "
              << ImportanceModelData::total_predicted_points_ << std::endl;

    // Reset:
    ImportanceModelData::total_missing_points_ = 0;
    ImportanceModelData::total_predicted_points_ = 0;

    o << cur_match.p1() << "," << cur_match.p2() << ","
      << cur_match.match_title() << "," << iid_win_prob << ","
      << non_iid_win_prob << "," << basic_win_prob << ","
      << cur_match.ServeWinProbabilityIID(cur_match.p1()) << ","
      << cur_match.ServeWinProbabilityIID(cur_match.p2()) << "," << average_sets
      << "," << average_sets_iid << "," << average_games << ","
      << average_games_iid << std::endl;
  }
  o.close();
}

void test_importance_version() {
  std::vector<ImportanceModelData> m = ImportanceModelData::ImportFromFile(
      "/vol/bitbucket/mdi14/atp_points_predicted_imp.csv");
}

void verbose_test_run() {
  std::vector<ModelData> m =
      ModelData::ImportFromFile("atp_points_predicted.csv");

  ModelData test = m[2];

  bool bo5 = true;

  const unsigned int kSimulations = 1;

  AdjustedMCModel adj(test.p1(), test.p2(), bo5, test, kSimulations, true);

  std::map<std::string, double> iid_probs;

  iid_probs[test.p1()] = test.ServeWinProbabilityIID(test.p1());
  iid_probs[test.p2()] = test.ServeWinProbabilityIID(test.p2());

  IIDMCModel iid_model(test.p1(), test.p2(), bo5, iid_probs, kSimulations);
}

void verbose_test_run_importance() {
  std::vector<ImportanceModelData> m =
      ImportanceModelData::ImportFromFile("atp_points_predicted_imp.csv");

  ImportanceModelData test =
      *std::find_if(m.begin(), m.end(), [](const ImportanceModelData &m) {
        return m.p1() == "Adrian Mannarino" && m.p2() == "Fabio Fognini";
      });

  bool bo5 = true;

  const unsigned int kSimulations = 1;

  ImportanceMCModel adj(test.p1(), test.p2(), bo5, test, kSimulations, true);

  std::cout << ImportanceModelData::total_missing_points_ << " out of "
            << ImportanceModelData::total_predicted_points_ << std::endl;

  std::map<std::string, double> iid_probs;

  iid_probs[test.p1()] = test.ServeWinProbabilityIID(test.p1());
  iid_probs[test.p2()] = test.ServeWinProbabilityIID(test.p2());

  IIDMCModel iid_model(test.p1(), test.p2(), bo5, iid_probs, kSimulations);
}

int main() { test_importance_model(); }
