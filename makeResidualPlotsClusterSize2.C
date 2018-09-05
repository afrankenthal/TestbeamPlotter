TLegend * makeLegend(bool ROC, TH1F * plot1, TH1F * plot2, TH1F * plot3) { 
				TString leg25, leg50, leg150;
				if (ROC) {
								leg25 = "PSI46, 25x100";
								leg50 = "PSI46, 50x50";
								leg150 = "PSI46, 150x100";
				}
				else {
								leg25 = "PROC, 25x100";
								leg50 = "PROC, 50x50";
								leg150 = "PROC, 150x100";
				}

				TLegend * legend = new TLegend(0.15, 0.85, 0.4, 0.7);
				legend->AddEntry(plot1, leg25, "l");
				//legend->AddEntry(plot2, leg50, "l");
				legend->AddEntry(plot3, leg150, "l");
				return legend;
}

void makeOnePlot(bool ROC, TH1F * plot1, TH1F * plot2, TH1F * plot3) {
				plot1->Scale(1/plot1->GetEntries());
				plot2->Scale(1/plot2->GetEntries());
				plot3->Scale(1/plot3->GetEntries());

				plot2->SetLineColor(kRed);
				plot3->SetLineColor(kGreen);

				TCanvas * newC = new TCanvas();

				plot1->GetXaxis()->SetRangeUser(-100,100);

				plot1->Draw("HIST");
				plot2->Draw("HIST SAME");
				plot3->Draw("HIST SAME");

				TLegend * leg = makeLegend(ROC, plot1, plot2, plot3);

				leg->Draw();
}

void makeResidualPlotsClusterSize2() { 

				gStyle->SetOptStat(0);
				gStyle->SetOptFit(1);

				TFile * file1 = TFile::Open("../Data/Chewie_Runs10617_10681_Weinan_v3.root");
				TFile * file2 = TFile::Open("../Data/Chewie_Runs10408_10418.root");
				TFile * file3 = TFile::Open("../Data/Chewie_Runs1924_1964.root");
				TFile * file4 = TFile::Open("../Data/Chewie_Runs1539_1547_DUT1_Weinan.root");

				//////////////////////////////////////
				TH1F * psi46_25x100_X = (TH1F*)file1->Get("Resolution/Dut0/XResiduals/hXResidualsClusterSize2_Dut0");
				TH1F * psi46_50x50_X = (TH1F*)file1->Get("Resolution/Dut1/XResiduals/hXResidualsClusterSize2_Dut1");
				TH1F * psi46_150x100_X = (TH1F*)file4->Get("Resolution/Dut0/XResiduals/hXResidualsClusterSize2_Dut0");

				//makeLegend(1, psi46_25x100_X, psi46_50x50_X, psi46_150x100_X);

				psi46_25x100_X->Scale(1/psi46_25x100_X->GetEntries());
				psi46_50x50_X->Scale(1/psi46_50x50_X->GetEntries());
				psi46_150x100_X->Scale(1/psi46_150x100_X->GetEntries());

				psi46_50x50_X->SetLineColor(kRed);
				psi46_150x100_X->SetLineColor(kGreen);

				TCanvas * cPSIX = new TCanvas();

				psi46_25x100_X->GetXaxis()->SetRangeUser(-100,100);
				psi46_25x100_X->GetYaxis()->SetRangeUser(0,0.25);
				psi46_25x100_X->GetXaxis()->SetTitle("X residual (#mum)");
				psi46_25x100_X->GetXaxis()->SetTitleOffset(1.2);
				psi46_25x100_X->GetYaxis()->SetTitle("A.U.");
				psi46_25x100_X->SetTitle("X residuals with cluster size 2");

				psi46_25x100_X->Fit("gaus");
				psi46_25x100_X->GetFunction("gaus")->SetLineColor(kBlue);
				//psi46_50x50_X->Fit("gaus");
				//psi46_50x50_X->GetFunction("gaus")->SetLineColor(kRed);
				psi46_150x100_X->Fit("gaus", "", "sames");
				psi46_150x100_X->GetFunction("gaus")->SetLineColor(kGreen);

				cPSIX->Update();

				TPaveStats * st = (TPaveStats*)psi46_25x100_X->FindObject("stats");
				st->SetTextColor(kBlue);
				st->SetX1NDC(.65);
				st->SetX2NDC(.85);
				st->SetY1NDC(.85);
				st->SetY2NDC(.65);
				
				TPaveStats * st2 = (TPaveStats*)psi46_150x100_X->FindObject("stats");
				st2->SetTextColor(kGreen+1);
				st2->SetX1NDC(.65);
				st2->SetX2NDC(.85);
				st2->SetY1NDC(0.6);
				st2->SetY2NDC(0.4);

				cPSIX->Modified();

				TLegend * leg1 = makeLegend(1, psi46_25x100_X, psi46_50x50_X, psi46_150x100_X);

				leg1->Draw();


				/////////////////////////////////////////
				TH1F * psi46_25x100_Y = (TH1F*)file1->Get("Resolution/Dut0/YResiduals/hYResidualsClusterSize2_Dut0");
				TH1F * psi46_50x50_Y = (TH1F*)file1->Get("Resolution/Dut1/YResiduals/hYResidualsClusterSize2_Dut1");
				TH1F * psi46_150x100_Y = (TH1F*)file4->Get("Resolution/Dut0/YResiduals/hYResidualsClusterSize2_Dut0");

				psi46_25x100_Y->Scale(1/psi46_25x100_Y->GetEntries());
				psi46_50x50_Y->Scale(1/psi46_50x50_Y->GetEntries());
				psi46_150x100_Y->Scale(1/psi46_150x100_Y->GetEntries());

				psi46_50x50_Y->SetLineColor(kRed);
				psi46_150x100_Y->SetLineColor(kGreen);

				TCanvas * cPSIY = new TCanvas();

				psi46_25x100_Y->GetXaxis()->SetRangeUser(-100,100);
				psi46_25x100_Y->GetYaxis()->SetRangeUser(0,0.2);
				psi46_25x100_Y->GetXaxis()->SetTitle("Y residual (#mum)");
				psi46_25x100_Y->GetXaxis()->SetTitleOffset(1.2);
				psi46_25x100_Y->GetYaxis()->SetTitle("A.U.");
				psi46_25x100_Y->SetTitle("Y residuals with cluster size 2");

				psi46_25x100_Y->Fit("gaus");
				psi46_25x100_Y->GetFunction("gaus")->SetLineColor(kBlue);
				//psi46_50x50_X->Fit("gaus");
				//psi46_50x50_X->GetFunction("gaus")->SetLineColor(kRed);
				psi46_150x100_Y->Fit("gaus", "", "sames");
				psi46_150x100_Y->GetFunction("gaus")->SetLineColor(kGreen);

				cPSIY->Update();

				st = (TPaveStats*)psi46_25x100_Y->FindObject("stats");
				st->SetTextColor(kBlue);
				st->SetX1NDC(.65);
				st->SetX2NDC(.85);
				st->SetY1NDC(.85);
				st->SetY2NDC(.65);
				
				st2 = (TPaveStats*)psi46_150x100_Y->FindObject("stats");
				st2->SetTextColor(kGreen+1);
				st2->SetX1NDC(.65);
				st2->SetX2NDC(.85);
				st2->SetY1NDC(0.6);
				st2->SetY2NDC(0.4);

				cPSIY->Modified();
				
				//psi46_25x100_Y->Draw("HIST");
				//psi46_50x50_Y->Draw("HIST SAME");
				//psi46_150x100_Y->Draw("HIST SAME");

				TLegend * leg2 = makeLegend(1, psi46_25x100_Y, psi46_50x50_Y, psi46_150x100_Y);

				leg2->Draw();

				/////////////////////////////////////////
				TH1F * proc_25x100_X = (TH1F*)file2->Get("Resolution/Dut0/XResiduals/hXResidualsClusterSize2_Dut0");
				TH1F * proc_50x50_X = (TH1F*)file2->Get("Resolution/Dut1/XResiduals/hXResidualsClusterSize2_Dut1");
				TH1F * proc_150x100_X = (TH1F*)file3->Get("Resolution/Dut1/XResiduals/hXResidualsClusterSize2_Dut1");

				proc_25x100_X->Scale(1/proc_25x100_X->GetEntries());
				proc_50x50_X->Scale(1/proc_50x50_X->GetEntries());
				proc_150x100_X->Scale(1/proc_150x100_X->GetEntries());

				proc_50x50_X->SetLineColor(kRed);
				proc_150x100_X->SetLineColor(kGreen);

				TCanvas * cPROCX = new TCanvas();

				proc_25x100_X->GetXaxis()->SetRangeUser(-100,100);
				proc_25x100_X->GetYaxis()->SetRangeUser(0,0.2);
				proc_25x100_X->GetXaxis()->SetTitle("X residual (#mum)");
				proc_25x100_X->GetXaxis()->SetTitleOffset(1.2);
				proc_25x100_X->GetYaxis()->SetTitle("A.U.");
				proc_25x100_X->SetTitle("X residuals with cluster size 2");
				
				proc_25x100_X->Fit("gaus");
				proc_25x100_X->GetFunction("gaus")->SetLineColor(kBlue);
				//psi46_50x50_X->Fit("gaus");
				//psi46_50x50_X->GetFunction("gaus")->SetLineColor(kRed);
				proc_150x100_X->Fit("gaus", "", "sames");
				proc_150x100_X->GetFunction("gaus")->SetLineColor(kGreen);

				cPROCX->Update();

				st = (TPaveStats*)proc_25x100_X->FindObject("stats");
				st->SetTextColor(kBlue);
				st->SetX1NDC(.65);
				st->SetX2NDC(.85);
				st->SetY1NDC(.85);
				st->SetY2NDC(.65);
				
				st2 = (TPaveStats*)proc_150x100_X->FindObject("stats");
				st2->SetTextColor(kGreen+1);
				st2->SetX1NDC(.65);
				st2->SetX2NDC(.85);
				st2->SetY1NDC(0.6);
				st2->SetY2NDC(0.4);

				cPROCX->Modified();

				TLegend * leg3 = makeLegend(0, proc_25x100_X, proc_50x50_X, proc_150x100_X);

				leg3->Draw();

				/////////////////////////////////////////
				TH1F * proc_25x100_Y = (TH1F*)file2->Get("Resolution/Dut0/YResiduals/hYResidualsClusterSize2_Dut0");
				TH1F * proc_50x50_Y = (TH1F*)file2->Get("Resolution/Dut1/YResiduals/hYResidualsClusterSize2_Dut1");
				TH1F * proc_150x100_Y = (TH1F*)file3->Get("Resolution/Dut1/YResiduals/hYResidualsClusterSize2_Dut1");

				proc_25x100_Y->Scale(1/proc_25x100_Y->GetEntries());
				proc_50x50_Y->Scale(1/proc_50x50_Y->GetEntries());
				proc_150x100_Y->Scale(1/proc_150x100_Y->GetEntries());

				proc_50x50_Y->SetLineColor(kRed);
				proc_150x100_Y->SetLineColor(kGreen);

				TCanvas * cPROCY = new TCanvas();

				proc_25x100_Y->GetXaxis()->SetRangeUser(-100,100);
				proc_25x100_Y->GetYaxis()->SetRangeUser(0,0.2);
				proc_25x100_Y->GetXaxis()->SetTitle("Y residual (#mum)");
				proc_25x100_Y->GetXaxis()->SetTitleOffset(1.2);
				proc_25x100_Y->GetYaxis()->SetTitle("A.U.");
				proc_25x100_Y->SetTitle("Y residuals with cluster size 2");
				
				proc_25x100_Y->Fit("gaus");
				proc_25x100_Y->GetFunction("gaus")->SetLineColor(kBlue);
				//psi46_50x50_X->Fit("gaus");
				//psi46_50x50_X->GetFunction("gaus")->SetLineColor(kRed);
				proc_150x100_Y->Fit("gaus", "", "sames");
				proc_150x100_Y->GetFunction("gaus")->SetLineColor(kGreen);

				cPROCY->Update();

				st = (TPaveStats*)proc_25x100_Y->FindObject("stats");
				st->SetTextColor(kBlue);
				st->SetX1NDC(.65);
				st->SetX2NDC(.85);
				st->SetY1NDC(.85);
				st->SetY2NDC(.65);
				
				st2 = (TPaveStats*)proc_150x100_Y->FindObject("stats");
				st2->SetTextColor(kGreen+1);
				st2->SetX1NDC(.65);
				st2->SetX2NDC(.85);
				st2->SetY1NDC(0.6);
				st2->SetY2NDC(0.4);

				cPROCY->Modified();

				TLegend * leg4 = makeLegend(0, proc_25x100_Y, proc_50x50_Y, proc_150x100_Y);

				leg4->Draw();
}
