#include <ROOT/RDataFrame.hxx>
#include <TFile.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TDirectory.h>
#include <TString.h>
#include <iostream>
#include <map>
#include <vector>
#include <mutex>

// Structure for collecting all waveforms per (slot,chan)
struct WaveformEntry {
    std::vector<std::vector<double>> waveforms;
};

// Helper to group consecutive samples of the same (slot,chan)
std::map<std::pair<int,int>, std::vector<std::vector<double>>> 
GroupWaveforms(const std::vector<uint32_t>& slot,
               const std::vector<uint32_t>& chan,
               const std::vector<uint32_t>& waveform)
{
    std::map<std::pair<int,int>, std::vector<std::vector<double>>> grouped;
    if (slot.empty()) return grouped;

    std::pair<int,int> current_key = {slot[0], chan[0]};
    std::vector<double> current_waveform;

    for (size_t i = 0; i < waveform.size(); ++i) {
        std::pair<int,int> key = {slot[i], chan[i]};
        if (key != current_key && !current_waveform.empty()) {
            grouped[current_key].push_back(current_waveform);
            current_waveform.clear();
            current_key = key;
        }
        current_waveform.push_back(static_cast<double>(waveform[i]));
    }

    // Push last waveform
    if (!current_waveform.empty()) grouped[current_key].push_back(current_waveform);
    return grouped;
}

// Get or create nested directories in output file
TDirectory* GetOrCreateDir(TFile* fout, int slot, int chan) {
    fout->cd();
    TString slot_dir_name = Form("slot_%d", slot);
    TDirectory *slot_dir = (TDirectory*)fout->Get(slot_dir_name);
    if (!slot_dir) slot_dir = fout->mkdir(slot_dir_name);

    slot_dir->cd();
    TString chan_dir_name = Form("chan_%d", chan);
    TDirectory *chan_dir = (TDirectory*)slot_dir->Get(chan_dir_name);
    if (!chan_dir) chan_dir = slot_dir->mkdir(chan_dir_name);

    return chan_dir;
}

void plot_waveform_rdf(const char* input_file = "waveform_tree.root",
                         const char* output_file = "waveform_plots.root",
                         Long64_t evt_start = 0,
                         Long64_t evt_end = -1)
{
    ROOT::EnableImplicitMT();
    gROOT->SetBatch(kTRUE);

    ROOT::RDataFrame df("waveform_tree", input_file);
    auto nentries = *df.Count();
    if (evt_end < 0 || evt_end > nentries) evt_end = nentries;

    std::cout << "Processing events " << evt_start << " to " << evt_end - 1 << std::endl;

    std::map<std::pair<int,int>, WaveformEntry> waveform_map;
    std::mutex map_mutex;

    Long64_t evt_counter = 0;
    df.ForeachSlot(
        [&](unsigned int /*slotID*/,
            const std::vector<uint32_t> &ev_slot,
            const std::vector<uint32_t> &ev_chan,
            const std::vector<uint32_t> &ev_waveform)
        {
            if (evt_counter < evt_start || (evt_end >= 0 && evt_counter >= evt_end))
                return;
            evt_counter++;
    
            auto grouped = GroupWaveforms(ev_slot, ev_chan, ev_waveform);
            std::lock_guard<std::mutex> lock(map_mutex);
            for (auto &kv : grouped)
                for (auto &wf : kv.second)
                    waveform_map[kv.first].waveforms.push_back(wf);
        },
        {"slot", "chan", "waveform"});

    // Output file
    TFile *fout = TFile::Open(output_file, "RECREATE");

    // Loop over channels
    for (auto &entry : waveform_map) {
        int slot = entry.first.first;
        int chan = entry.first.second;
        auto &wf_list = entry.second.waveforms;
        if (wf_list.empty()) continue;

        TDirectory *chan_dir = GetOrCreateDir(fout, slot, chan);
        chan_dir->cd();

        // Overlay all waveforms
        TCanvas *c_all = new TCanvas(Form("overlay_all_s%d_c%d", slot, chan),
                                     Form("Overlay Slot %d Chan %d", slot, chan), 800, 600);
        c_all->cd();

        double ymin = 1e9, ymax = -1e9;
        for (const auto &wf : wf_list)
            for (auto v : wf) { ymin = std::min(ymin, v); ymax = std::max(ymax, v); }

        bool first = true;
        for (size_t i = 0; i < wf_list.size(); ++i) {
            const auto &wf = wf_list[i];
            std::vector<double> x(wf.size());
            for (size_t k = 0; k < wf.size(); ++k) x[k] = k;
            TGraph *gr = new TGraph(wf.size(), x.data(), wf.data());
            gr->SetLineColor((i % 9) + 1);
            gr->SetLineWidth(1);
            if (first) {
                gr->SetTitle(Form("Slot %d Chan %d", slot, chan));
                gr->GetXaxis()->SetTitle("Sample");
                gr->GetYaxis()->SetTitle("ADC");
                gr->GetYaxis()->SetRangeUser(ymin - 10, ymax + 10);
                gr->Draw("AL");
                first = false;
            } else {
                gr->Draw("LP SAME");
            }
        }
        c_all->Write();

        // Grid of first 16 waveforms
        TCanvas *c_grid = new TCanvas(Form("grid16_s%d_c%d", slot, chan),
                                      Form("Grid Slot %d Chan %d", slot, chan), 1000, 800);
        c_grid->Divide(4, 4);
        size_t nplot = std::min<size_t>(wf_list.size(), 16);
        for (size_t i = 0; i < nplot; ++i) {
            c_grid->cd(i + 1);
            const auto &wf = wf_list[i];
            std::vector<double> x(wf.size());
            for (size_t k = 0; k < wf.size(); ++k) x[k] = k;
	    TGraph *gr = new TGraph(wf.size(), x.data(), wf.data());
            gr->SetLineColor(1);
            gr->SetTitle(Form("Evt %zu", i));
            gr->GetXaxis()->SetTitle("Sample");
            gr->GetYaxis()->SetTitle("ADC");
            gr->Draw("ALP");
        }
        c_grid->Write();
    }

    fout->Write();
    fout->Close();
    std::cout << "Saved plots to " << output_file << std::endl;
}

