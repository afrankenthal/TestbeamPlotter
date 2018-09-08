///////// Helper functions and structs

typedef struct SensorInfo {
    int dut;
    int pitchX;
    int pitchY;
    TString roc;
} SensorInfo;

SensorInfo proc25 = {0, 25, 100, "PROC600"};
SensorInfo proc50 = {1, 50, 50, "PROC600"};
SensorInfo proc150 = {1, 150, 100, "PROC600"};
SensorInfo psi25 = {0, 25, 100, "PSI46"};
SensorInfo psi50 = {1, 50, 50, "PSI46"};
SensorInfo psi150 = {0, 150, 100, "PSI46"};

enum PlotDirection { X, Y };

TString FormTitle(SensorInfo sensor, TString name) {
    if (sensor.dut == -1)
        return name;
    return Form("DUT %d (%dx%d #mum^{2}) %s", sensor.dut, sensor.pitchX, sensor.pitchY, (const char *)name);
}

TString FormXaxis(SensorInfo sensor, TString name) {
    if (sensor.dut == -1)
        return name;
    return Form("%s (pitch: %d#mum)", (const char *)name, sensor.pitchX);
}

TString FormYaxis(SensorInfo sensor, TString name) {
    if (sensor.dut == -1)
        return name;
    return Form("%s (pitch: %d#mum)", (const char *)name, sensor.pitchY);
}


///// Generic plotting functions

void compose1DPlot(TGraphAsymmErrors * plot, TString title, TString Xaxis, TString Yaxis, SensorInfo sensor) {
    plot->SetTitle(FormTitle(sensor, title));
    plot->GetXaxis()->SetTitle(Xaxis);
    plot->GetXaxis()->SetTitleOffset(1.25);
    plot->GetYaxis()->SetTitle(Yaxis);
    plot->GetYaxis()->SetTitleOffset(1.25);
    if (title.Contains("X Cell Charge")) {
        plot->Draw("AP");
    }
    else {
        plot->SetMarkerStyle(20);
        plot->SetLineColor(9);
        plot->Draw("AP");
        plot->GetHistogram()->SetMinimum(0.3);
        plot->GetHistogram()->SetMaximum(1.05);
        gPad->Update();
        gPad->Modified();
    }
}

void compose1DPlot(TH1F * plot, TString title, TString Xaxis, TString Yaxis, SensorInfo sensor) {
    plot->SetTitle(FormTitle(sensor, title));
    plot->GetXaxis()->SetTitle(Xaxis);
    plot->GetXaxis()->SetTitleOffset(1.25);
    plot->GetYaxis()->SetTitle(Yaxis);
    plot->GetYaxis()->SetTitleOffset(1.25);
    if (title.Contains("X Cell Charge") || title.Contains("Y Cell Charge") || title.Contains("Residual")) {
        plot->SetStats(0);
        plot->Draw();
        gPad->Update();
        gPad->Modified();
    }
    else {
        plot->Draw();
        gPad->Update();
        TPaveStats * st = (TPaveStats*)plot->FindObject("stats");
        st->SetX1NDC(0.7);
        st->SetY1NDC(0.82);
        st->SetX2NDC(0.86);
        st->SetY2NDC(0.87);
        gPad->Modified();
    }
}

void compose2DPlot(TH2F * plot, TString title, TString Xaxis, TString Yaxis, TString Zaxis, SensorInfo sensor) {
    plot->SetTitle(FormTitle(sensor, title));
    plot->SetTitleSize(0.1, "t");
    plot->SetTitleOffset(0.8);
    plot->GetXaxis()->SetTitle(FormXaxis(sensor, Xaxis));
    plot->GetXaxis()->SetTitleOffset(1.25);
    plot->GetYaxis()->SetTitle(FormYaxis(sensor, Yaxis));
    plot->GetZaxis()->SetTitle(Zaxis);
    plot->GetZaxis()->SetTitleOffset(1.5);
}

void compose1DCanvas() {
    TCanvas * canvas = new TCanvas();
    //gPad->SetGridx();
    //gPad->SetGridy();
    gPad->SetTicks();
    gStyle->SetTitleY(0.97);
    gStyle->SetTitleH(0.05);
    canvas->SetLeftMargin(0.13);
}

void compose2DCanvas(TH2F * plot) {
    TCanvas * canvas = new TCanvas();
    canvas->SetRightMargin(0.16);
    gStyle->SetOptStat(10);
    gStyle->SetTitleY(0.97);
    gStyle->SetTitleH(0.05);
    plot->Draw("COLZ");
    gPad->SetTicks();
    gPad->Update();
    TPaveStats * st = (TPaveStats*)plot->FindObject("stats");
    st->SetX1NDC(0.82);
    st->SetY1NDC(0.92);
    st->SetX2NDC(0.93);
    st->SetY2NDC(0.96);
    //st->SetX1NDC(0.83);
    //st->SetY1NDC(0.91);
    //st->SetX2NDC(0.93);
    //st->SetY2NDC(0.95);
    gPad->Modified();
}

////// Make all the relevant plots

void joinWindowPlots(TFile * f, SensorInfo sensor, TH2F ** hsum) {
    f->cd(Form("Windows/Dut%d", sensor.dut));
    *hsum = 0;
    TIter next(gDirectory->GetListOfKeys());
    TKey *key;
    while ((key = (TKey*)next())) {
        TClass *cl = gROOT->GetClass(key->GetClassName());
        if (!cl->InheritsFrom("TH1")) continue;
        TH1 *h = (TH1*)key->ReadObj();
        TString name = (TString)h->GetName();
        TString comp = Form("Dut%d_Window_Run", sensor.dut);
        if (name.Contains(comp)) {
            std::cout << name << std::endl;
            if (!*hsum) {
                *hsum = (TH2F*)h->Clone();
                (*hsum)->SetDirectory(0);
            }
            else
                (*hsum)->Add(h);
        }
    }
}

void makeWindowPlots(TFile * f, SensorInfo sensor) {
    TH2F * plot = 0;
    if (!plot)
        std::cout << "initial plot empty" << std::endl;
    joinWindowPlots(f, sensor, &plot);
    if (!plot)
        std::cout << "final  plot empty" << std::endl;
    compose2DPlot(plot, "Hit Distribution", "Column #", "Row #", "Number of hits", sensor);
    compose2DCanvas(plot);
}

void makePlaneEfficiencyPlots(TH2F * plot, SensorInfo sensor) {
    compose2DPlot(plot, "Efficiency Distribution", "Column #", "Row #", "Efficiency", sensor);
    compose2DCanvas(plot);
    plot->SetMinimum(0.94);
    TPaveStats * st = (TPaveStats*)plot->FindObject("stats");
    st->SetX1NDC(0.82);
    st->SetY1NDC(0.92);
    st->SetX2NDC(0.92);
    st->SetY2NDC(0.96);
    plot->GetYaxis()->SetTitleOffset(1.25);
}

void makeCellEfficiencyPlots(TH2F * plot, SensorInfo sensor) {
    if (!sensor.dut)
        plot->SetMinimum(0.99);
    else
        plot->SetMinimum(0.97);
    compose2DPlot(plot, "Cell Efficiency Distribution", "Track X coordinate", "Track Y coordinate", "Efficiency", sensor);
    compose2DCanvas(plot);
    plot->SetStats(0);
    plot->GetZaxis()->SetTitleOffset(1.4);
    plot->GetYaxis()->SetTitleOffset(1.25);
}

void make1DXCellEfficiencyPlot(TH1F * plot, SensorInfo sensor, int numHits) {
    compose1DCanvas();
    compose1DPlot(plot, "X Cell Efficiency", "Track X coordinate (pitch: 25 #mum)", "Efficiency", sensor);
    TPaveText * hits = new TPaveText(0.65, 0.15, 0.85, 0.2, "NDC");
    hits->AddText(Form("Using %d hit", numHits) + (numHits > 1 ? TString("s") : TString("")));
    hits->Draw("SAME");
}

void make1DXYCellEfficiencyPlots(TH1F * plot1, TH1F * plot2, TH1F * plot4, SensorInfo sensor, bool XorY) {
    compose1DCanvas();

    plot1->SetMarkerStyle(20);
    plot1->SetLineColor(9);
    plot1->SetMarkerColor(9);
    plot2->SetMarkerStyle(20);
    plot2->SetLineColor(46);
    plot2->SetMarkerColor(46);
    plot4->SetMarkerStyle(20);
    plot4->SetLineColor(30);
    plot4->SetMarkerColor(30);

    if (XorY)
        compose1DPlot(plot1, "X Cell Efficiency", FormXaxis(sensor, "Track X coordinate"), "Efficiency", sensor);
    else
        compose1DPlot(plot1, "Y Cell Efficiency", FormYaxis(sensor, "Track Y coordinate"), "Efficiency", sensor);
    plot2->Draw("P");
    plot4->Draw("P");

    TLine * yLine = new TLine(0, 0.3, 0, 1.04);
    yLine->SetLineStyle(9);
    yLine->Draw();

    TLegend * legend = new TLegend(0.62, 0.13, 0.87, 0.26);
    legend->AddEntry(plot1, "Using 1 hit", "lep");
    legend->AddEntry(plot2, "Using 2 hits", "lep");
    legend->AddEntry(plot4, "Using 4 hits", "lep");
    legend->Draw();
}

void makeClusterSizePlots(TH1F * plot, SensorInfo sensor) {
    compose1DCanvas();
    compose1DPlot(plot, "Cluster Size Distribution", "Cluster size", "Events", sensor);
    gPad->SetLogy();
    gPad->Update();
    TPaveStats * stats = (TPaveStats*)plot->FindObject("stats");
    stats->SetOptStat(10);
    gPad->Modified();
    gPad->Update();
}

void makeChargePlots(TH1F * plot, SensorInfo sensor) {
    compose1DCanvas();
    compose1DPlot(plot, "Charge Distribution", "Number of electrons", "Events", sensor);
    gPad->Update();
    plot->GetYaxis()->SetTitleOffset(1.4);
    TPaveStats * st = (TPaveStats*)plot->FindObject("stats");
    st->SetOptStat(1111);
    st->SetX1NDC(0.6);
    st->SetY1NDC(0.72);
    st->SetX2NDC(0.86);
    st->SetY2NDC(0.87);
    gPad->Modified();
    gPad->Update();
}

void make2DChargePlots(TH2F * plot, SensorInfo sensor) {
    compose2DCanvas(plot);
    compose2DPlot(plot, "2D Charge Distribution", "Column #", "Row #", "Number of electrons", sensor);
    plot->Draw("COLZ");
    TPaveStats * st = (TPaveStats*)plot->FindObject("stats");
    st->SetX1NDC(0.82);
    st->SetY1NDC(0.92);
    st->SetX2NDC(0.92);
    st->SetY2NDC(0.96);
    gPad->Modified();
    gPad->Update();
}

void make2DCellChargePlots(TH2F * plot, SensorInfo sensor) {
    compose2DCanvas(plot);
    compose2DPlot(plot, "Cell Charge 2D Distribution", "Track X coordinate", "Track Y coordinate", "Number of electrons", sensor);
    plot->SetStats(0);
    plot->Draw("COLZ");
    gPad->Update();
    gPad->Modified();
}

void make1DXYCellChargePlot(TH1F * plot, SensorInfo sensor, int numHits, PlotDirection dir) {
    compose1DCanvas();

    if (dir == X)
        compose1DPlot(plot, "X Cell Charge", FormXaxis(sensor, "Track X coordinate"), "Number of electrons", sensor);
    else
        compose1DPlot(plot, "Y Cell Charge", FormYaxis(sensor, "Track Y coordinate"), "Number of electrons", sensor);

    plot->GetYaxis()->SetTitleOffset(1.5);
    plot->SetMinimum(0);
    TPaveText * hits = new TPaveText(0.65, 0.2, 0.85, 0.25, "NDC");
    hits->AddText(Form("Using %d hit", numHits) + (numHits > 1 ? TString("s") : TString("")));
    hits->Draw("SAME");
}

void make1DXYCellChargePlots(TH1F * plot1, TH1F * plot2, TH1F * plot4, SensorInfo sensor, PlotDirection dir) {

    compose1DCanvas();

    plot1->SetMarkerStyle(20);
    plot1->SetLineColor(9);
    plot1->SetMarkerColor(9);
    plot2->SetMarkerStyle(20);
    plot2->SetLineColor(46);
    plot2->SetMarkerColor(46);
    plot4->SetMarkerStyle(20);
    plot4->SetLineColor(30);
    plot4->SetMarkerColor(30);

    if (dir == X)
        compose1DPlot(plot1, "X Cell Charge", FormXaxis(sensor, "Track X coordinate"), "Number of electrons", sensor);
    else
        compose1DPlot(plot1, "Y Cell Charge", FormYaxis(sensor, "Track Y coordinate"), "Number of electrons", sensor);

    plot1->SetMaximum(18000);
    plot1->SetMinimum(0);
    plot2->Draw("SAME");
    if (sensor.pitchX != 150)
        plot4->Draw("SAME");

    plot1->GetYaxis()->SetTitleOffset(1.5);

    TLine * yLine = new TLine(0, 0, 0, 18000);
    yLine->SetLineStyle(9);
    yLine->Draw();

    TLegend * legend = new TLegend(0.62, 0.13, 0.87, 0.26);
    legend->AddEntry(plot1, "Using 1 hit", "lep");
    legend->AddEntry(plot2, "Using 2 hits", "lep");
    if (sensor.pitchX != 150)
        legend->AddEntry(plot4, "Using 4 hits", "lep");
    legend->Draw();
}

void make2DXYCellChargePlots(TH2F * plot1, TH2F * plot2, TH2F * plot4, SensorInfo sensor, PlotDirection dir) {

    TCanvas * canvas = new TCanvas(Form("c2D%sCellDut%d", dir ? "Y" : "X", sensor.dut), "", 900, 500);

    TPaveLabel * title;
    title = new TPaveLabel(0.1,0.94,0.9,0.99, Form("Predicted Cell Charge vs. %s Coordinate - DUT %d", dir ? "Y" : "X", sensor.dut),"NB");

    title->SetFillStyle(0);
    title->SetLineColor(0);
    title->Draw();
    TPad* graphPad = new TPad("Graphs","Graphs",0.01,0.01,0.99,0.93);
    graphPad->Draw();
    graphPad->Divide(3, 1, 0.005, 0.005);
    graphPad->SetBottomMargin(0);
    graphPad->SetTopMargin(0);
    graphPad->cd(1);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.005);
    plot1->SetTitle("1 Hit");
    plot1->Draw("COL");
    gPad->Update();
    TPaveStats * st = (TPaveStats*)plot1->FindObject("stats");
    st->SetX1NDC(0.6);
    st->SetY1NDC(0.8);
    st->SetX2NDC(0.9);
    st->SetY2NDC(0.88);
    plot1->GetYaxis()->SetTitleOffset(2.2);
    gPad->Modified();
    graphPad->cd(2);
    plot2->SetTitle("2 Hits");
    plot2->GetYaxis()->SetTitle("");
    plot2->GetYaxis()->SetLabelSize(0);
    plot2->GetYaxis()->SetLabelOffset(999);
    plot2->Draw("COL");
    gPad->Update();
    st = (TPaveStats*)plot2->FindObject("stats");
    st->SetX1NDC(0.5);
    st->SetY1NDC(0.8);
    st->SetX2NDC(0.8);
    st->SetY2NDC(0.88);
    gPad->Modified();
    graphPad->cd(3);
    plot4->SetTitle("4 Hits");
    gPad->SetLeftMargin(0.00);
    gPad->SetRightMargin(0.15);
    plot4->GetYaxis()->SetTitle("");
    plot4->GetYaxis()->SetLabelSize(0);
    plot4->GetYaxis()->SetLabelOffset(999);
    plot4->Draw("COLZ");
    gPad->Update();
    st = (TPaveStats*)plot4->FindObject("stats");
    st->SetX1NDC(0.5);
    st->SetY1NDC(0.8);
    st->SetX2NDC(0.8);
    st->SetY2NDC(0.88);
    plot4->GetZaxis()->SetTitle("Events");
    plot4->GetZaxis()->SetTitleOffset(1.5);
    gPad->Modified();
}

void make4CellChargePlotsDut0(TH2F * plot, SensorInfo sensor) {
    compose2DCanvas(plot);
    compose2DPlot(plot, "4-Cell 2D Charge Distribution", "Track X coordinate", "Track Y coordinate", "Number of electrons", sensor);
    plot->SetStats(0);
    plot->Draw("COLZ");
    const char * labelsX[10] = {"-10", "-5", "0", "5", "10", "-10", "-5", "0", "5", "10"};
    for (int i = 1; i <= 10; i++)
        plot->GetXaxis()->SetBinLabel(i, labelsX[i-1]);
    plot->GetXaxis()->SetLabelSize(.045);
    Int_t labelBinsY[10] = {1, 5, 10, 15, 20, 21, 25, 30, 35, 40};
    const char * labelsY[10] = {"-50", "-25", "0", "25", "50", "-50", "-25", "0", "25", "50"};
    for (int i = 0; i < 10; i++)
        plot->GetYaxis()->SetBinLabel(labelBinsY[i], labelsY[i]);
    plot->GetYaxis()->SetLabelSize(.045);
    TLine * yLine = new TLine(0, -100, 0, 100);
    yLine->SetLineStyle(9);
    yLine->Draw();
    TLine * xLine = new TLine(-25, 0, 25, 0);
    xLine->SetLineStyle(9);
    xLine->Draw();
    gPad->Update();
    gPad->Modified();
}

void make4CellChargePlots(TH2F * plot, SensorInfo sensor) {
    compose2DCanvas(plot);
    compose2DPlot(plot, "4-Cell 2D Charge Distribution", "Track X coordinate", "Track Y coordinate", "Number of electrons", sensor);
    plot->SetStats(0);
    plot->Draw("COLZ");
    //Int_t labelBinsX[10] = {1, 3, 5, 7, 9, 12, 14, 16, 18, 20};
    //const char * labelsX[10] = {"-23", "-13", "-3", "8", "18", "-18", "-8", "3", "13", "23"};
    //for (int i = 0; i < 10; i++)
    //	plot->GetXaxis()->SetBinLabel(labelBinsX[i], labelsX[i]);
    //plot->GetXaxis()->SetLabelSize(.045);
    //Int_t labelBinsY[10] = {1, 3, 5, 7, 9, 12, 14, 16, 18, 20};
    //const char * labelsY[10] = {"-23", "-13", "-3", "8", "18", "-18", "-8", "3", "13", "23"};
    //for (int i = 0; i < 10; i++)
    //	plot->GetYaxis()->SetBinLabel(labelBinsY[i], labelsY[i]);
    //plot->GetYaxis()->SetLabelSize(.045);

    TLine * yLine;
    if (sensor.pitchX == 150)
        yLine = new TLine(0, -sensor.pitchY/2, 0, sensor.pitchY/2);
    else if (sensor.pitchX == 25 || sensor.pitchX == 50)
        yLine = new TLine(0, -sensor.pitchY, 0, sensor.pitchY);
    yLine->SetLineStyle(9);
    yLine->Draw();
    TLine * xLine;
    if (sensor.pitchX == 150)
        xLine = new TLine(-sensor.pitchX/2, 0, sensor.pitchX/2, 0);
    else if (sensor.pitchX == 25 || sensor.pitchX == 50)
        xLine = new TLine(-sensor.pitchX, 0, sensor.pitchX, 0);
    xLine->SetLineStyle(9);
    xLine->Draw();
    gPad->Update();
    gPad->Modified();
}

void makeResidualPlots(TH1F * res0, TH1F * res1, PlotDirection dir, TString title) {
    compose1DCanvas();
    Double_t norm = 1;
    res0->Scale(norm/res0->Integral(), "width");
    norm = 1;
    res1->Scale(norm/res1->Integral(), "width");
    if (title.Contains("Cluster"))
        res1->GetXaxis()->SetRangeUser(-100,100);

    res1->GetYaxis()->SetRangeUser(0, 0.05);

    res1->SetLineColor(46);
    res1->Draw("HIST");
    res0->Draw("HIST SAME");

    res1->SetTitle(title);
    res1->GetYaxis()->SetTitle("A.U.");
    res1->GetYaxis()->SetTitleOffset(1.50);

    if (dir == X)
        res1->GetXaxis()->SetTitle("X residual (#mum)");
    else
        res1->GetXaxis()->SetTitle("Y residual (#mum)");

    res1->GetXaxis()->SetTitleOffset(1.25);

    gPad->Update();
    res1->SetStats(0);

    TLegend * legend = new TLegend(0.65, 0.77, 0.87, 0.87);
    legend->AddEntry(res0, "DUT 0 (25x100 #mum^{2})");
    legend->AddEntry(res1, "DUT 1 (50x50 #mum^{2})");
    legend->Draw();

    gPad->Modified();
    gPad->Update();
}

void makePlots() {

    //TFile * file = TFile::Open("../Data/Chewie_Runs1539_1547_DUT1_Weinan.root");
    //TFile * file = TFile::Open("../Data/Chewie_Runs10617_10681_Weinan_v2.root");
    //TFile * file = TFile::Open("../Data/Chewie_Runs10617_10681_Weinan_v3.root");
    TFile * file = TFile::Open("../../TB_Data/ChewieOutput/Chewie_Runs10408_10418.root");
    //TFile * file = TFile::Open("../Data/Chewie_Runs1924_1964.root");
    SensorInfo sensor0 = proc25;
    SensorInfo sensor1 = proc50;

    // 2D Window plots
    makeWindowPlots(file, sensor0);
    makeWindowPlots(file, sensor1);

    //// Efficiency plots
    TH2F * plot;

    //// 2D Efficiency plots
    //plot = (TH2F*)file->Get("Efficiency/Dut0/Efficiency/2DEfficiency_Dut0");
    //makePlaneEfficiencyPlots(plot, sensor0);
    //plot = (TH2F*)file->Get("Efficiency/Dut1/Efficiency/2DEfficiency_Dut1");
    //makePlaneEfficiencyPlots(plot, sensor1);

    ////// Cell Efficiency plots
    //plot = (TH2F*)file->Get("Efficiency/Dut0/CellEfficiency/hCellEfficiency_Dut0");
    //makeCellEfficiencyPlots(plot, sensor0);
    //plot = (TH2F*)file->Get("Efficiency/Dut1/CellEfficiency/hCellEfficiency_Dut1");
    //makeCellEfficiencyPlots(plot, sensor1);

    //// 1D X Cell Efficiency plots
    //TH1F * cellPlot1Hit = (TH1F*)file->Get("Efficiency/Dut0/Efficiency/h1DXcellEfficiencyFirstHit_Dut0");
    ////make1DXCellEfficiencyPlot(cellNumPlot1Hit, cellNormPlot1Hit, sensor0, 1);
    //TH1F * cellPlot2Hits = (TH1F*)file->Get("Efficiency/Dut0/Efficiency/h1DXcellEfficiencySecondHit_Dut0");
    ////make1DXCellEfficiencyPlot(cellNumPlot2Hits, cellNormPlot2Hits, sensor0, 2);
    //TH1F * cellPlot4Hits = (TH1F*)file->Get("Efficiency/Dut0/Efficiency/h1DXcellEfficiencyFourthHit_Dut0");
    ////make1DXCellEfficiencyPlot(cellNumPlot4Hits, cellNormPlot4Hits, sensor0, 4);
    //make1DXYCellEfficiencyPlots(cellPlot1Hit, cellPlot2Hits, cellPlot4Hits, sensor0, 1);

    //cellPlot1Hit = (TH1F*)file->Get("Efficiency/Dut1/Efficiency/h1DXcellEfficiencyFirstHit_Dut1");
    ////make1DXCellEfficiencyPlot(cellNumPlot1Hit, cellNormPlot1Hit, sensor1, 1);
    //cellPlot2Hits = (TH1F*)file->Get("Efficiency/Dut1/Efficiency/h1DXcellEfficiencySecondHit_Dut1");
    ////make1DXCellEfficiencyPlot(cellNumPlot2Hits, cellNormPlot2Hits, sensor1, 2);
    //cellPlot4Hits = (TH1F*)file->Get("Efficiency/Dut1/Efficiency/h1DXcellEfficiencyFourthHit_Dut1");
    ////make1DXCellEfficiencyPlot(cellNumPlot4Hits, cellNormPlot4Hits, sensor1, 4);
    //make1DXYCellEfficiencyPlots(cellPlot1Hit, cellPlot2Hits, cellPlot4Hits, sensor1, 1);

    //// 1D Y Cell Efficiency plots
    //cellPlot1Hit = (TH1F*)file->Get("Efficiency/Dut0/Efficiency/h1DYcellEfficiencyFirstHit_Dut0");
    ////make1DXCellEfficiencyPlot(cellNumPlot1Hit, cellNormPlot1Hit, sensor0, 1);
    //cellPlot2Hits = (TH1F*)file->Get("Efficiency/Dut0/Efficiency/h1DYcellEfficiencySecondHit_Dut0");
    ////make1DXCellEfficiencyPlot(cellNumPlot2Hits, cellNormPlot2Hits, sensor0, 2);
    //cellPlot4Hits = (TH1F*)file->Get("Efficiency/Dut0/Efficiency/h1DYcellEfficiencyFourthHit_Dut0");
    ////make1DXCellEfficiencyPlot(cellNumPlot4Hits, cellNormPlot4Hits, sensor0, 4);
    //make1DXYCellEfficiencyPlots(cellPlot1Hit, cellPlot2Hits, cellPlot4Hits, sensor0, 0);

    //cellPlot1Hit = (TH1F*)file->Get("Efficiency/Dut1/Efficiency/h1DYcellEfficiencyFirstHit_Dut1");
    ////make1DXCellEfficiencyPlot(cellNumPlot1Hit, cellNormPlot1Hit, sensor1, 1);
    //cellPlot2Hits = (TH1F*)file->Get("Efficiency/Dut1/Efficiency/h1DYcellEfficiencySecondHit_Dut1");
    ////make1DXCellEfficiencyPlot(cellNumPlot2Hits, cellNormPlot2Hits, sensor1, 2);
    //cellPlot4Hits = (TH1F*)file->Get("Efficiency/Dut1/Efficiency/h1DYcellEfficiencyFourthHit_Dut1");
    ////make1DXCellEfficiencyPlot(cellNumPlot4Hits, cellNormPlot4Hits, sensor1, 4);
    //make1DXYCellEfficiencyPlots(cellPlot1Hit, cellPlot2Hits, cellPlot4Hits, sensor1, 0);

    // Cluster size plots
    TH1F * csPlot = (TH1F*)file->Get("Charge/Dut0/ClusterSize/hClusterSize_Dut0");
    makeClusterSizePlots(csPlot, sensor0);
    csPlot = (TH1F*)file->Get("Charge/Dut1/ClusterSize/hClusterSize_Dut1");
    makeClusterSizePlots(csPlot, sensor1);

    // Charge distribution plots
    TH1F * chargePlot = (TH1F*)file->Get("Charge/Dut0/Landau/hCellLandau_Dut0");
    makeChargePlots(chargePlot, sensor0);
    chargePlot = (TH1F*)file->Get("Charge/Dut1/Landau/hCellLandau_Dut1");
    makeChargePlots(chargePlot, sensor1);

    // 2D Charge distribution plots
    //TH2F * charge2DPlot = (TH2F*)file->Get("Charge/Dut0/2DCharge/h2DCharge_Dut0");
    //make2DChargePlots(charge2DPlot, sensor0);
    //charge2DPlot = (TH2F*)file->Get("Charge/Dut1/2DCharge/h2DCharge_Dut1");
    //make2DChargePlots(charge2DPlot, sensor1);

    // 2D cell charge distribution plots
    TH2F * cellChargePlot = (TH2F*)file->Get("Charge/Dut0/2DCellCharge/h2DCellCharge_Dut0");
    make2DCellChargePlots(cellChargePlot, sensor0);
    cellChargePlot = (TH2F*)file->Get("Charge/Dut1/2DCellCharge/h2DCellCharge_Dut1");
    make2DCellChargePlots(cellChargePlot, sensor1);

    // 1D X cell charge plots
    TH1F * XcellChargePlot1Hit = (TH1F*)file->Get("Charge/Dut0/XcellCharge1D/h1DXcellCharge_Dut0");
    //make1DXYCellChargePlot(XcellChargePlot1Hit, sensor0, 1, X);
    TH1F * XcellChargePlot2Hits = (TH1F*)file->Get("Charge/Dut0/XcellCharge1D/h1DXcellChargeSecondHit_Dut0");
    //make1DXYCellChargePlot(XcellChargePlot2Hits, sensor0, 2, X);
    TH1F * XcellChargePlot4Hits = (TH1F*)file->Get("Charge/Dut0/XcellCharge1D/h1DXcellChargeFourthHit_Dut0");
    //make1DXYCellChargePlot(XcellChargePlot4Hits, sensor0, 4, X);
    make1DXYCellChargePlots(XcellChargePlot1Hit, XcellChargePlot2Hits, XcellChargePlot4Hits, sensor0, X);

    XcellChargePlot1Hit = (TH1F*)file->Get("Charge/Dut1/XcellCharge1D/h1DXcellCharge_Dut1");
    //make1DXYCellChargePlot(XcellChargePlot1Hit, sensor1, X);
    XcellChargePlot2Hits = (TH1F*)file->Get("Charge/Dut1/XcellCharge1D/h1DXcellChargeSecondHit_Dut1");
    //make1DXYCellChargePlot(XcellChargePlot2Hits, sensor1, 2, X);
    XcellChargePlot4Hits = (TH1F*)file->Get("Charge/Dut1/XcellCharge1D/h1DXcellChargeFourthHit_Dut1");
    //make1DXYCellChargePlot(XcellChargePlot4Hits, sensor1, 4, X);
    make1DXYCellChargePlots(XcellChargePlot1Hit, XcellChargePlot2Hits, XcellChargePlot4Hits, sensor1, X);

    // 1D Y cell charge plots
    TH1F * YcellChargePlot1Hit = (TH1F*)file->Get("Charge/Dut0/YcellCharge1D/h1DYcellCharge_Dut0");
    //make1DXYCellChargePlot(YcellChargePlot1Hit, sensor0, 1, Y);
    TH1F * YcellChargePlot2Hits = (TH1F*)file->Get("Charge/Dut0/YcellCharge1D/h1DYcellChargeSecondHit_Dut0");
    //make1DXYCellChargePlot(YcellChargePlot2Hits, sensor0, 2, Y);
    TH1F * YcellChargePlot4Hits = (TH1F*)file->Get("Charge/Dut0/YcellCharge1D/h1DYcellChargeFourthHit_Dut0");
    //make1DXYCellChargePlot(YcellChargePlot4Hits, sensor0, 4, Y);
    make1DXYCellChargePlots(YcellChargePlot1Hit, YcellChargePlot2Hits, YcellChargePlot4Hits, sensor0, Y);

    YcellChargePlot1Hit = (TH1F*)file->Get("Charge/Dut1/YcellCharge1D/h1DYcellCharge_Dut1");
    //make1DXYCellChargePlot(YcellChargePlot1Hit, sensor1, 1, Y);
    YcellChargePlot2Hits = (TH1F*)file->Get("Charge/Dut1/YcellCharge1D/h1DYcellChargeSecondHit_Dut1");
    //make1DXYCellChargePlot(YcellChargePlot2Hits, sensor1, 2, Y);
    YcellChargePlot4Hits = (TH1F*)file->Get("Charge/Dut1/YcellCharge1D/h1DYcellChargeFourthHit_Dut1");
    //make1DXYCellChargePlot(YcellChargePlot4Hits, sensor1, 4, Y);
    make1DXYCellChargePlots(YcellChargePlot1Hit, YcellChargePlot2Hits, YcellChargePlot4Hits, sensor1, Y);

    // 2D X cell charge plots
    TH2F * X2DcellChargePlot1Hit = (TH2F*)file->Get("Charge/Dut0/XcellCharge2D/h2DXcellCharge_Dut0");
    //make1DXYCellChargePlot(XcellChargePlot1Hit, sensor0, 1, X);
    TH2F * X2DcellChargePlot2Hits = (TH2F*)file->Get("Charge/Dut0/XcellCharge2D/h2DXcellChargeSecondHit_Dut0");
    //make1DXYCellChargePlot(XcellChargePlot2Hits, sensor0, 2, X);
    TH2F * X2DcellChargePlot4Hits = (TH2F*)file->Get("Charge/Dut0/XcellCharge2D/h2DXcellChargeFourthHit_Dut0");
    //make1DXYCellChargePlot(XcellChargePlot4Hits, sensor0, 4, X);
    make2DXYCellChargePlots(X2DcellChargePlot1Hit, X2DcellChargePlot2Hits, X2DcellChargePlot4Hits, sensor0, X);

    X2DcellChargePlot1Hit = (TH2F*)file->Get("Charge/Dut1/XcellCharge2D/h2DXcellCharge_Dut1");
    //make1DXYCellChargePlot(XcellChargePlot1Hit, sensor1, 1, X);
    X2DcellChargePlot2Hits = (TH2F*)file->Get("Charge/Dut1/XcellCharge2D/h2DXcellChargeSecondHit_Dut1");
    //make1DXYCellChargePlot(XcellChargePlot2Hits, sensor1, 2, X);
    X2DcellChargePlot4Hits = (TH2F*)file->Get("Charge/Dut1/XcellCharge2D/h2DXcellChargeFourthHit_Dut1");
    //make1DXYCellChargePlot(XcellChargePlot4Hits, sensor1, 4, X);
    make2DXYCellChargePlots(X2DcellChargePlot1Hit, X2DcellChargePlot2Hits, X2DcellChargePlot4Hits, sensor1, X);

    // 2D Y cell charge plots
    TH2F * Y2DcellChargePlot1Hit = (TH2F*)file->Get("Charge/Dut0/YcellCharge2D/h2DYcellCharge_Dut0");
    //make1DXYCellChargePlot(XcellChargePlot1Hit, sensor0, 1, Y);
    TH2F * Y2DcellChargePlot2Hits = (TH2F*)file->Get("Charge/Dut0/YcellCharge2D/h2DYcellChargeSecondHit_Dut0");
    //make1DXYCellChargePlot(XcellChargePlot2Hits, sensor0, 2, Y);
    TH2F * Y2DcellChargePlot4Hits = (TH2F*)file->Get("Charge/Dut0/YcellCharge2D/h2DYcellChargeFourthHit_Dut0");
    //make1DXYCellChargePlot(XcellChargePlot4Hits, sensor0, 4, Y);
    make2DXYCellChargePlots(Y2DcellChargePlot1Hit, Y2DcellChargePlot2Hits, Y2DcellChargePlot4Hits, sensor0, Y);

    Y2DcellChargePlot1Hit = (TH2F*)file->Get("Charge/Dut1/YcellCharge2D/h2DYcellCharge_Dut1");
    //make1DXYCellChargePlot(XcellChargePlot1Hit, sensor1, 1, Y);
    Y2DcellChargePlot2Hits = (TH2F*)file->Get("Charge/Dut1/YcellCharge2D/h2DYcellChargeSecondHit_Dut1");
    //make1DXYCellChargePlot(XcellChargePlot2Hits, sensor1, 2, Y);
    Y2DcellChargePlot4Hits = (TH2F*)file->Get("Charge/Dut1/YcellCharge2D/h2DYcellChargeFourthHit_Dut1");
    //make1DXYCellChargePlot(XcellChargePlot4Hits, sensor1, 4, Y);
    make2DXYCellChargePlots(Y2DcellChargePlot1Hit, Y2DcellChargePlot2Hits, Y2DcellChargePlot4Hits, sensor1, Y);

    // 4-cell 2D charge plots
    TH2F * cells4Charge2DPlot = (TH2F*)file->Get("Charge/Dut0/2DCellCharge/h4CellsCharge_Dut0");
    make4CellChargePlots(cells4Charge2DPlot, sensor0);
    cells4Charge2DPlot = (TH2F*)file->Get("Charge/Dut1/2DCellCharge/h4CellsCharge_Dut1");
    make4CellChargePlots(cells4Charge2DPlot, sensor1);

    // X residual plots
    TH1F * resPlotDut0 = (TH1F*)file->Get("Resolution/Dut0/XResiduals/hXResidualsClusterSize2_Dut0");
    TH1F * resPlotDut1 = (TH1F*)file->Get("Resolution/Dut1/XResiduals/hXResidualsClusterSize2_Dut1");
    makeResidualPlots(resPlotDut0, resPlotDut1, X, "X Residuals -- Cluster Size 2");

    resPlotDut0 = (TH1F*)file->Get("Resolution/Dut0/XResiduals/hXResidualsClusterSize1_Dut0");
    resPlotDut1 = (TH1F*)file->Get("Resolution/Dut1/XResiduals/hXResidualsClusterSize1_Dut1");
    makeResidualPlots(resPlotDut0, resPlotDut1, X, "X Residuals -- Cluster Size 1");

    // Y residual plots
    resPlotDut0 = (TH1F*)file->Get("Resolution/Dut0/YResiduals/hYResidualsClusterSize2_Dut0");
    resPlotDut1 = (TH1F*)file->Get("Resolution/Dut1/YResiduals/hYResidualsClusterSize2_Dut1");
    makeResidualPlots(resPlotDut0, resPlotDut1, Y, "Y Residuals -- Cluster Size 2");

    resPlotDut0 = (TH1F*)file->Get("Resolution/Dut0/YResiduals/hYResidualsClusterSize1_Dut0");
    resPlotDut1 = (TH1F*)file->Get("Resolution/Dut1/YResiduals/hYResidualsClusterSize1_Dut1");
    makeResidualPlots(resPlotDut0, resPlotDut1, Y, "Y Residuals -- Cluster Size 1");
}
