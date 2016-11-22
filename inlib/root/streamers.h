// Copyright (C) 2010, Guy Barrand. All rights reserved.
// See the file inlib.license for terms.

#ifndef inlib_root_streamers
#define inlib_root_streamers

#include "named.h"
#include "inlib/root/date.h"

#include "directory.h"
#include "file.h"
#include "graph.h"
#include "clss.h"
#include "info.h"
#include "dummy.h"

#include "../sout.h"
#include "../vmanip.h"

//#include "../histo/histo_data.h"
#include "../histo/profile_data.h"

#include "../histo/h1d.h"
#include "../histo/h2d.h"
#include "../histo/h3d.h"
#include "../histo/p1d.h"
#include "../histo/p2d.h"

#include "../histo/h1df.h"
#include "../histo/h2df.h"
#include "../histo/h3df.h"

#include <list>
#include <cmath> //::log10, ::fabs.
//#include "../rtausmed.h"

namespace inlib {
    namespace root {

        typedef inlib::histo::histo_data<double, unsigned int, unsigned int, double> hd_data;
        typedef histo::histo_data<double, unsigned int, unsigned int, float> hf_data;
        typedef inlib::histo::profile_data<double, unsigned int, unsigned int, double, double> pd_data;

        inline bool List_empty_stream(buffer& a_buffer)
        {
            unsigned int beg;

            if (!a_buffer.write_version(4, beg)) return false;

            if (!Object_stream(a_buffer)) return false;

            std::string name;

            if (!a_buffer.write(name)) return false;

            int nobjects = 0;

            if (!a_buffer.write(nobjects)) return false;

            if (!a_buffer.set_byte_count(beg)) return false;

            return true;
        }

        inline bool AttAxis_stream_read(buffer& a_buffer)
        {
            int fNdivisions = 510;       //Number of divisions(10000*n3 + 100*n2 + n1)
            short fAxisColor = 1;        //color of the line axis
            short fLabelColor = 1;       //color of labels
            short fLabelFont = 62;       //font for labels
            float fLabelOffset = 0.005F; //offset of labels
            float fLabelSize = 0.04F;    //size of labels
            float fTickLength = 0.03F;   //length of tick marks
            float fTitleOffset = 1;      //offset of axis title
            float fTitleSize = 0.04F;    //size of axis title
            short fTitleColor = 1;       //color of axis title
            short fTitleFont = 62;       //font for axis title
            // Version 4 streaming (ROOT/v3-00-6).
            short v;
            unsigned int s, c;

            if (!a_buffer.read_version(v, s, c)) return false;

            if (!a_buffer.read(fNdivisions)) return false;

            if (!a_buffer.read(fAxisColor)) return false;

            if (!a_buffer.read(fLabelColor)) return false;

            if (!a_buffer.read(fLabelFont)) return false;

            if (!a_buffer.read(fLabelOffset)) return false;

            if (!a_buffer.read(fLabelSize)) return false;

            if (!a_buffer.read(fTickLength)) return false;

            if (!a_buffer.read(fTitleOffset)) return false;

            if (!a_buffer.read(fTitleSize)) return false;

            if (!a_buffer.read(fTitleColor)) return false;

            if (!a_buffer.read(fTitleFont)) return false;

            if (!a_buffer.check_byte_count(s, c, "TAttAxis")) return false;

            return true;
        }

        inline bool AttAxis_stream_write(buffer& a_buffer)
        {
            int fNdivisions = 510;   //Number of divisions(10000*n3 + 100*n2 + n1)
            short fAxisColor = 1;    //color of the line axis
            short fLabelColor = 1;   //color of labels
            short fLabelFont = 62;    //font for labels
            float fLabelOffset = 0.005F;  //offset of labels
            float fLabelSize = 0.04F;    //size of labels
            float fTickLength = 0.03F;   //length of tick marks
            float fTitleOffset = 1;  //offset of axis title
            float fTitleSize = 0.04F;    //size of axis title
            short fTitleColor = 1;   //color of axis title
            short fTitleFont = 62;    //font for axis title
            // Version 4 streaming (ROOT/v3-00-6).
            unsigned int beg;

            if (!a_buffer.write_version(4, beg)) return false;

            if (!a_buffer.write(fNdivisions)) return false;

            if (!a_buffer.write(fAxisColor)) return false;

            if (!a_buffer.write(fLabelColor)) return false;

            if (!a_buffer.write(fLabelFont)) return false;

            if (!a_buffer.write(fLabelOffset)) return false;

            if (!a_buffer.write(fLabelSize)) return false;

            if (!a_buffer.write(fTickLength)) return false;

            if (!a_buffer.write(fTitleOffset)) return false;

            if (!a_buffer.write(fTitleSize)) return false;

            if (!a_buffer.write(fTitleColor)) return false;

            if (!a_buffer.write(fTitleFont)) return false;

            if (!a_buffer.set_byte_count(beg)) return false;

            return true;
        }

        template <class HIST>
        inline std::string axis_title(const HIST& a_h,
                                      const std::string& a_key)
        {
            typedef std::map<std::string, std::string> annotations_t;
            annotations_t::const_iterator it = a_h.annotations().find(a_key);

            if (it == a_h.annotations().end()) return std::string();

            return (*it).second;
        }

        class dummy_fac : public virtual ifac {
        public: //ifac
            virtual iro* create(const std::string& a_class, const args& a_args)
            {
                if (inlib::rcmp(a_class, "TGraph")) { //for TH_read_1D/List.
                    return new graph();
                    // for read_sinfos() :
                } else if (inlib::rcmp(a_class, "TStreamerInfo")) {
                    return new StreamerInfo(*this);
                } else if (inlib::rcmp(a_class, "TObjArray")) {
                    std::string* sc = ifac::arg_class(a_args);

                    if (sc) {
                        if ((*sc) == streamer_element::s_class()) {
                            return new ObjArray<streamer_element>(*this, true);
                        } else {
                            m_out << "inlib::rroot::dummy_fac::create :"
                                  << " Can't create TObjArray of " << *sc << "."
                                  << std::endl;
                            return 0;
                        }
                    } else {
                        return new iros(*this, true);
                    }
                } else if (inlib::rcmp(a_class, "TStreamerBase")
                           || inlib::rcmp(a_class, "TStreamerBasicType")
                           || inlib::rcmp(a_class, "TStreamerBasicPointer")
                           || inlib::rcmp(a_class, "TStreamerObjectAny")
                           || inlib::rcmp(a_class, "TStreamerObject")
                           || inlib::rcmp(a_class, "TStreamerObjectPointer")
                           || inlib::rcmp(a_class, "TStreamerString")
                           || inlib::rcmp(a_class, "TStreamerSTL")
                           || inlib::rcmp(a_class, "TStreamerLoop")
                           || inlib::rcmp(a_class, "TList")
                          ) {
                    return new dummy_streamer_element();
                } else {
                    m_out << "inlib::rroot::dummy_fac::create :"
                          << " dummy. Can't create object of class " << sout(a_class) << "."
                          << std::endl;
                }

                return 0;
            }
            virtual void destroy(iro*& a_obj)
            {
                delete a_obj;
                a_obj = 0;
            }
        public:
            dummy_fac(std::ostream& a_out): m_out(a_out) {}
            virtual ~dummy_fac() {}
        protected:
            dummy_fac(const dummy_fac& a_from): ifac(a_from), m_out(a_from.m_out) {}
            dummy_fac& operator=(const dummy_fac&)
            {
                return *this;
            }
        protected:
            std::ostream& m_out;
        };

        inline bool Axis_stream(buffer& a_buffer, inlib::histo::axis<double, unsigned int>& a_fAxis)
        {
            // Version 6 streaming (ROOT/v3-00-6).
            short v;
            unsigned int s, c;

            if (!a_buffer.read_version(v, s, c)) return false;

            std::string name;
            std::string title;

            if (!Named_stream(a_buffer, name, title)) return false;

            if (!AttAxis_stream(a_buffer)) return false;

            int number;

            if (!a_buffer.read(number)) return false;

            double min;

            if (!a_buffer.read(min)) return false;

            double max;

            if (!a_buffer.read(max)) return false;

            //::printf("debug : BatchLab::RioTH::streamTAxis : %d %g %g\n",
            //  number,min,max);
            std::vector<double> edges;

            if (!Array_stream<double>(a_buffer, edges)) return false; //fXbins TArrayD

            int edgen = edges.size();

            if (edgen <= 0) {
                a_fAxis.configure(number, min, max);
            } else {
                std::vector<double> vedges;

                for (int index = 0; index < edgen; index++) {
                    vedges.push_back(edges[index]);
                }

                a_fAxis.configure(vedges);
            }

            int First;

            if (!a_buffer.read(First)) return false;

            int Last;

            if (!a_buffer.read(Last)) return false;

            if (v >= 8) { //fBits2.
                unsigned short dummy;

                if (!a_buffer.read(dummy)) return false;
            }

            //Bool_t
            unsigned char TimeDisplay;

            if (!a_buffer.read(TimeDisplay)) return false;

            //TString
            std::string TimeFormat;

            if (!a_buffer.read(TimeFormat)) return false;

            if (v >= 7) {
                //THashList*
                dummy_fac fac(a_buffer.out());

                if (!dummy_TXxx_pointer_stream(a_buffer, fac)) return false;
            }

            if (!a_buffer.check_byte_count(s, c, "TAxis")) return false;

            return true;
        }

        inline bool axis_stream(buffer& a_buffer,
                                const histo::axis<double, unsigned int>& a_axis,
                                const std::string& a_name,
                                const std::string& a_title)
        {
            // Version 6 streaming (ROOT/v3-00-6).
            unsigned int beg;

            if (!a_buffer.write_version(6, beg)) return false;

            if (!Named_stream(a_buffer, a_name, a_title)) return false;

            if (!AttAxis_stream(a_buffer)) return false;

            if (!a_buffer.write(a_axis.bins())) return false;

            if (!a_buffer.write(a_axis.lower_edge())) return false;

            if (!a_buffer.write(a_axis.upper_edge())) return false;

            // fXbins
            //if(a_axis.m_fixed) {
            //  std::vector<double> v;
            //  ArrayT<double> dummy(v);
            //  if(!dummy.stream(a_buffer)) return false; //TArrayD
            //} else {
            if (!a_buffer.write_array(a_axis.edges())) return false; //TArrayD

            //}

            if (!a_buffer.write((int)0)) return false; //fFirst

            if (!a_buffer.write((int)0)) return false; //fLast

            //Bool_t
            if (!a_buffer.write((unsigned char)0)) return false; //TimeDisplay

            //TString
            if (!a_buffer.write(std::string())) return false; //TimeFormat

            if (!a_buffer.set_byte_count(beg)) return false;

            return true;
        }

        inline bool null_epsil(double a_1, double a_2, double a_prec = -5)
        {
            return (::log10(::fabs(a_1 - a_2)) < a_prec ? true : false);
        }

        inline bool TH_read_1D(buffer& a_buffer, hd_data& a_data,
                               double& a_entries, double& a_Sw, double& a_Sw2, double& a_Sxw, double& a_Sx2w)
        {
            a_entries = 0;
            a_Sw = 0;
            a_Sw2 = 0;
            a_Sxw = 0;
            a_Sx2w = 0;
            unsigned int s, c;
            short vers;

            if (!a_buffer.read_version(vers, s, c)) return false;

            //::printf("debug : BatchLab::Rio::TH::streamTH1 : version %d\n",vers);
            // Version 3 streaming (ROOT/v3-00-6).
            std::string name;
            std::string title;

            if (!Named_stream(a_buffer, name, title)) return false;

            a_data.m_title = title;
            {
                short color, style, width;

                if (!AttLine_stream(a_buffer, color, style, width)) return false;
            }
            {
                short color, style;

                if (!AttFill_stream(a_buffer, color, style)) return false;
            }

            if (!AttMarker_stream(a_buffer)) return false;

            int Ncells;

            if (!a_buffer.read(Ncells)) return false;

            //fXAxis
            if (!Axis_stream(a_buffer, a_data.m_axes[0])) return false;

            a_data.m_axes[0].m_offset = 1;

            if (a_data.m_dimension == 2) {
                if (!Axis_stream(a_buffer, a_data.m_axes[1])) return false; //fYAxis

                a_data.m_axes[1].m_offset =
                    a_data.m_axes[0].m_offset * (a_data.m_axes[0].bins() + 2);
                inlib::histo::axis<double, unsigned int> dummy;

                if (!Axis_stream(a_buffer, dummy)) return false; //fZAxis
            } else {
                inlib::histo::axis<double, unsigned int> dummy;

                if (!Axis_stream(a_buffer, dummy)) return false; //fYAxis

                if (!Axis_stream(a_buffer, dummy)) return false; //fZAxis
            }

            short barOffset;

            if (!a_buffer.read(barOffset)) return false;

            short barWidth;

            if (!a_buffer.read(barWidth)) return false;

            if (!a_buffer.read(a_entries)) return false;

            if (!a_buffer.read(a_Sw)) return false; //fTsumw

            //::printf("debug : BatchLab::Rio::TH::streamTH1 : \"%s\" %g %g\n",
            //         a_data.m_title.c_str(),fEntries,fSw);

            if (!a_buffer.read(a_Sw2)) return false;

            if (!a_buffer.read(a_Sxw)) return false;

            if (!a_buffer.read(a_Sx2w)) return false;

            double max;

            if (!a_buffer.read(max)) return false;

            double min;

            if (!a_buffer.read(min)) return false;

            double NormFactor;

            if (!a_buffer.read(NormFactor)) return false;

            {
                std::vector<double> v;

                if (!Array_stream<double>(a_buffer, v)) return false;
            } //fContour TArrayD
            std::vector<double> sumw2; //fSumw2 TArrayD

            if (!Array_stream<double>(a_buffer, sumw2)) return false;

            {
                std::string opt;

                if (!a_buffer.read(opt)) return false; //TString fOption

                //look if it is an "annotation trick" :
                //if(opt.size()&&(opt[0]==0)) {
                //  fAnnotation = opt.substr(1,opt.size()-1);
                //}
            }
            {
                dummy_fac fac(a_buffer.out());
                List dummy(fac, true);

                if (!dummy.stream(a_buffer)) {
                    a_buffer.out() << "inlib::rroot::TH_read_1D :"
                                   << " List stream failed."
                                   << std::endl;
                    return false;
                }
            } //Functions

            if (vers >= 4) {
                int BufferSize;

                if (!a_buffer.read(BufferSize)) return false;

                //Double_t* Buffer; //[fBufferSize]
                if (!dummy_array_stream<double>(a_buffer, BufferSize)) return false;
            }

            if (vers >= 7) {
                //EBinErrorOpt  fBinStatErrOpt;
                int dummy;

                if (!a_buffer.read(dummy)) return false;
            }

            // Add two for outflows.
            if (a_data.m_dimension == 1) {
                a_data.m_bin_number = a_data.m_axes[0].m_number_of_bins + 2;
            } else if (a_data.m_dimension == 2) {
                a_data.m_bin_number =
                    (a_data.m_axes[0].m_number_of_bins + 2) *
                    (a_data.m_axes[1].m_number_of_bins + 2);
            }

            unsigned int binn = a_data.m_bin_number;
            a_data.m_bin_Sw2.resize(binn);

            if (binn == sumw2.size()) {
                for (unsigned int index = 0; index < binn; index++) {
                    a_data.m_bin_Sw2[index] = sumw2[index];
                }
            } else {
                a_data.m_bin_Sw2.assign(binn, 0);
            }

            if (!a_buffer.check_byte_count(s, c, "TH")) return false;

            return true;
        }

        template <class HIST>
        inline bool TH_write_1D(buffer& a_buffer,
                                const HIST& a_h,
                                const std::string& a_name,
                                const std::vector<double>& a_bin_Sw2)
        {
            if (!a_buffer.write_version(3)) return false;

            if (!Named_stream(a_buffer, a_name, a_h.title())) return false;

            if (!AttLine_stream(a_buffer)) return false;

            if (!AttFill_stream(a_buffer)) return false;

            if (!AttMarker_stream(a_buffer)) return false;

            if (!a_buffer.write((int)a_h.get_bins())) return false;

            //fXAxis,fYAxis,fZAxis
            if (a_h.dimension() == 3) {
                {
                    histo::axis<double, unsigned int> haxis(a_h.get_axis(0));

                    if (!axis_stream(a_buffer, haxis, "xaxis", axis_title(a_h, histo::key_axis_x_title()))) return false;
                }
                {
                    histo::axis<double, unsigned int> haxis(a_h.get_axis(1));

                    if (!axis_stream(a_buffer, haxis, "yaxis", axis_title(a_h, histo::key_axis_y_title()))) return false;
                }
                {
                    histo::axis<double, unsigned int> haxis(a_h.get_axis(2));

                    if (!axis_stream(a_buffer, haxis, "zaxis", axis_title(a_h, histo::key_axis_z_title()))) return false;
                }
            } else if (a_h.dimension() == 2) {
                {
                    histo::axis<double, unsigned int> haxis(a_h.get_axis(0));

                    if (!axis_stream(a_buffer, haxis, "xaxis", axis_title(a_h, histo::key_axis_x_title()))) return false;
                }
                {
                    histo::axis<double, unsigned int> haxis(a_h.get_axis(1));

                    if (!axis_stream(a_buffer, haxis, "yaxis", axis_title(a_h, histo::key_axis_y_title()))) return false;
                }
                {
                    histo::axis<double, unsigned int> dummy;
                    dummy.configure(1, 0, 1);

                    if (!axis_stream(a_buffer, dummy, "zaxis", axis_title(a_h, histo::key_axis_z_title()))) return false;
                }
            } else if (a_h.dimension() == 1) {
                {
                    histo::axis<double, unsigned int> haxis(a_h.get_axis(0));

                    if (!axis_stream(a_buffer, haxis, "xaxis", axis_title(a_h, histo::key_axis_x_title()))) return false;
                }
                {
                    histo::axis<double, unsigned int> dummy;
                    dummy.configure(1, 0, 1);

                    if (!axis_stream(a_buffer, dummy, "yaxis", axis_title(a_h, histo::key_axis_y_title()))) return false;
                }
                {
                    histo::axis<double, unsigned int> dummy;
                    dummy.configure(1, 0, 1);

                    if (!axis_stream(a_buffer, dummy, "zaxis", axis_title(a_h, histo::key_axis_z_title()))) return false;
                }
            } else {
                return false;
            }

            if (!a_buffer.write((short)(1000 * 0.25))) return false; //fBarOffset

            if (!a_buffer.write((short)(1000 * 0.5))) return false; //fBarWidth

            if (!a_buffer.write((double)a_h.all_entries())) return false;

            if (!a_buffer.write((double)a_h.get_in_range_Sw())) return false; //enforce double in case h1df

            if (!a_buffer.write((double)a_h.get_in_range_Sw2())) return false; //idem

            {
                double value;
                a_h.get_ith_axis_Sxw(0, value);

                if (!a_buffer.write(value)) return false;
            }
            {
                double value;
                a_h.get_ith_axis_Sx2w(0, value);

                if (!a_buffer.write(value)) return false;
            }

            if (!a_buffer.write((double) - 1111)) return false; //fMaximum

            if (!a_buffer.write((double) - 1111)) return false; //fMinimum

            if (!a_buffer.write((double)0)) return false; //NormFactor

            if (!a_buffer.write_array(std::vector<double>())) return false; //fContour TArrayD

            if (!a_buffer.write_array(a_bin_Sw2)) return false; //fSumw2 TArrayD

            // store annotation on fOption
            // but try to fool CERN-ROOT in order that it does not
            // understand fOption as.. CERN-ROOT options !
            //{std::string opt = " "+fAnnotation;
            // opt[0] = 0; //awfull trick
            // if(!a_buffer.write(opt)) return false;} //TString fOption
            {
                std::string opt;

                if (!a_buffer.write(opt)) return false;
            } //TString fOption

            if (!List_empty_stream(a_buffer)) return false; //*TList fFunctions

            return true;
        }


        inline bool TH_read_2D(buffer& a_buffer, hd_data& a_data,
                               double& a_entries, double& a_Sw, double& a_Sw2,
                               double& a_Sxw, double& a_Sx2w, double& a_Syw, double& a_Sy2w)
        {
            unsigned int s, c;
            short v;

            if (!a_buffer.read_version(v, s, c)) return false;

            //::printf("debug : BatchLab::Rio::TH::streamTH2 : version %d\n",v);

            // Version 3 streaming (ROOT/v3-00-6).

            if (!TH_read_1D(a_buffer, a_data, a_entries, a_Sw, a_Sw2, a_Sxw, a_Sx2w)) return false;

            // the upper set :
            //data.m_title
            //data.m_bin_number
            //data.m_axes
            //data.m_bin_Sw2
            double ScaleFactor;

            if (!a_buffer.read(ScaleFactor)) return false;

            if (!a_buffer.read(a_Syw)) return false;

            if (!a_buffer.read(a_Sy2w)) return false;

            double Tsumwxy;

            if (!a_buffer.read(Tsumwxy)) return false;

            a_data.m_in_range_plane_Sxyw[0] = Tsumwxy;

            if (!a_buffer.check_byte_count(s, c, "TH2")) return false;

            return true;
        }

        template <class HIST>
        inline bool TH_write_2D(buffer& a_buffer,
                                const HIST& a_h,
                                const std::string& a_name,
                                const std::vector<double>& a_bin_Sw2)
        {
            if (!a_buffer.write_version(3)) return false;

            if (!TH_write_1D(a_buffer, a_h, a_name, a_bin_Sw2)) return false;

            if (!a_buffer.write((double)1)) return false; //ScaleFactor

            {
                double value;
                a_h.get_ith_axis_Sxw(1, value);

                if (!a_buffer.write(value)) return false;
            }
            {
                double value;
                a_h.get_ith_axis_Sx2w(1, value);

                if (!a_buffer.write(value)) return false;
            }

            if (!a_buffer.write((double)a_h.Sxyw())) return false; //Tsumwxy

            return true;
        }

        inline bool TH_read_3D(buffer& a_buffer, hd_data& a_data,
                               double& a_entries, double& a_Sw, double& a_Sw2,
                               double& a_Sxw, double& a_Sx2w,
                               double& a_Syw, double& a_Sy2w,
                               double& a_Szw, double& a_Sz2w)
        {
            unsigned int s, c;
            short v;

            if (!a_buffer.read_version(v, s, c)) return false;

            if (!TH_read_1D(a_buffer, a_data, a_entries, a_Sw, a_Sw2, a_Sxw, a_Sx2w)) return false;

            if (!Att3D_stream(a_buffer)) return false;

            // the upper set :
            //data.m_title
            //data.m_bin_number
            //data.m_axes
            //data.m_bin_Sw2

            if (!a_buffer.read(a_Syw)) return false;

            if (!a_buffer.read(a_Sy2w)) return false;

            double Tsumwxy;

            if (!a_buffer.read(Tsumwxy)) return false;

            if (!a_buffer.read(a_Szw)) return false;

            if (!a_buffer.read(a_Sz2w)) return false;

            double Tsumwxz;

            if (!a_buffer.read(Tsumwxz)) return false;

            double Tsumwyz;

            if (!a_buffer.read(Tsumwyz)) return false;

            a_data.m_in_range_plane_Sxyw[0] = Tsumwxy;
            a_data.m_in_range_plane_Sxyw[1] = Tsumwyz;
            a_data.m_in_range_plane_Sxyw[2] = Tsumwxz;

            if (!a_buffer.check_byte_count(s, c, "TH3")) return false;

            return true;
        }

        template <class HIST>
        inline bool TH_write_3D(buffer& a_buffer,
                                const HIST& a_h,
                                const std::string& a_name,
                                const std::vector<double>& a_bin_Sw2)
        {
            if (!a_buffer.write_version(4)) return false;

            if (!TH_write_1D(a_buffer, a_h, a_name, a_bin_Sw2)) return false;

            if (!Att3D_stream(a_buffer)) return false;

            {
                double value;
                a_h.get_ith_axis_Sxw(1, value);

                if (!a_buffer.write(value)) return false;
            }    //Tsumwy : Total Sum of weight*Y
            {
                double value;
                a_h.get_ith_axis_Sx2w(1, value);

                if (!a_buffer.write(value)) return false;
            }    //Tsumwy2 : Total Sum of weight*Y*Y

            if (!a_buffer.write((double)a_h.Sxyw())) return false; //Tsumwxy : Total Sum of weight*X*Y

            {
                double value;
                a_h.get_ith_axis_Sxw(2, value);

                if (!a_buffer.write(value)) return false;
            }    //Tsumwz : Total Sum of weight*Z
            {
                double value;
                a_h.get_ith_axis_Sx2w(2, value);

                if (!a_buffer.write(value)) return false;
            }    //Tsumwz2 : Total Sum of weight*Z*Z

            if (!a_buffer.write((double)a_h.Szxw())) return false; //Tsumwxz : Total Sum of weight*X*Z

            if (!a_buffer.write((double)a_h.Syzw())) return false; //Tsumwyz : Total Sum of weight*Y*Z

            return true;
        }

        inline inlib::histo::h1d* TH1F_stream_read(buffer& a_buffer)
        {
            unsigned int s, c;
            short v;

            if (!a_buffer.read_version(v, s, c)) return 0;

            //::printf("debug : BatchLab::Rio::TH1F::stream : version %d\n",v);
            // Version 1 streaming (ROOT/v3-00-6).
            // Now we have to reconstruct a valid Histogram from a_buffer :
            hd_data data;
            data.m_dimension = 1;
            //data.m_coords.resize(data.m_dimension,0);
            //data.m_ints.resize(data.m_dimension,0);
            data.m_axes.resize(1);
            double fEntries; //in range + outflow.
            double fSw;      //in range.
            double fSw2;     //in range.
            double fSxw;     //in range.
            double fSx2w;    //in range.

            if (!TH_read_1D(a_buffer, data, fEntries, fSw, fSw2, fSxw, fSx2w)) return 0;

            // the upper set :
            //data.m_title
            //data.m_bin_number
            //data.m_axes
            //data.m_bin_Sw2
            std::vector<float> bins; //fArray TArrayF

            if (!Array_stream<float>(a_buffer, bins)) return 0;

            if (!a_buffer.check_byte_count(s, c, "TH1F")) return 0;

            unsigned int binn = data.m_bin_number;
            //::printf("debug : BatchLab::Rio::TH1F::stream : histo bins %d\n",binn);
            data.m_bin_Sw.resize(binn, 0);
            {
                for (unsigned int index = 0; index < binn; index++) {
                    data.m_bin_Sw[index] = double(bins[index]);
                }
            }
            data.m_bin_entries.resize(binn, 0);
            {
                std::vector<double> empty;
                empty.resize(1, 0);
                data.m_bin_Sxw.resize(binn, empty);
                data.m_bin_Sx2w.resize(binn, empty);
            }
            data.m_all_entries = static_cast<unsigned int>(fEntries);
            data.m_in_range_entries = 0;
            data.m_in_range_Sw = fSw;
            data.m_in_range_Sw2 = fSw2;
            data.m_in_range_Sxw.resize(1, 0);
            data.m_in_range_Sx2w.resize(1, 0);
            data.m_in_range_Sxw[0] = fSxw;
            data.m_in_range_Sx2w[0] = fSx2w;
            inlib::histo::h1d* h = new inlib::histo::h1d("", 10, 0, 1);
            h->copy_from_data(data);
            return h; //give ownership to caller.
        }

        inline bool TH1F_stream_write(buffer& a_buffer,
                                const histo::h1df& a_h,
                                const std::string& a_name)
        {
            if (!a_buffer.write_version(1)) return false;

            if (!TH_write_1D(a_buffer, a_h, a_name, convert<float, double>(a_h.bins_sum_w2()))) return false;

            if (!a_buffer.write_array(a_h.bins_sum_w())) return false;

            return true;
        }

        inline bool TH1F_stream_write(buffer& a_buffer,
                                const histo::h1d& a_h,
                                const std::string& a_name)
        {
            if (!a_buffer.write_version(1)) return false;

            if (!TH_write_1D(a_buffer, a_h, a_name, a_h.bins_sum_w2())) return false;

            if (!a_buffer.write_array(convert<double, float>(a_h.bins_sum_w()))) return false;

            return true;
        }

        inline inlib::histo::h1d* TH1D_stream_read(buffer& a_buffer)
        {
            unsigned int s, c;
            short v;

            if (!a_buffer.read_version(v, s, c)) return 0;

            //::printf("debug : BatchLab::Rio::TH1D::stream : version %d\n",v);
            // Version 1 streaming (ROOT/v3-00-6).
            // Now we have to reconstruct a valid Histogram from a_buffer :
            hd_data data;
            data.m_dimension = 1;
            data.m_axes.resize(1);
            double fEntries; //in range + outflow.
            double fSw;      //in range.
            double fSw2;     //in range.
            double fSxw;     //in range.
            double fSx2w;    //in range.

            if (!TH_read_1D(a_buffer, data, fEntries, fSw, fSw2, fSxw, fSx2w)) return 0;

            // the upper set :
            //data.m_title
            //data.m_bin_number
            //data.m_axes
            //data.m_bin_Sw2
            std::vector<double> bins; //fArray TArrayD

            if (!Array_stream<double>(a_buffer, bins)) return 0;

            if (!a_buffer.check_byte_count(s, c, "TH1D")) return 0;

            unsigned int binn = data.m_bin_number;
            data.m_bin_Sw = bins;
            data.m_bin_entries.resize(binn, 0);
            {
                std::vector<double> empty;
                empty.resize(1, 0);
                data.m_bin_Sxw.resize(binn, empty);
                data.m_bin_Sx2w.resize(binn, empty);
            }
            data.m_all_entries = static_cast<unsigned int>(fEntries);
            data.m_in_range_entries = 0;
            data.m_in_range_Sw = fSw;
            data.m_in_range_Sw2 = fSw2;
            data.m_in_range_Sxw.resize(1, 0);
            data.m_in_range_Sx2w.resize(1, 0);
            data.m_in_range_Sxw[0] = fSxw;
            data.m_in_range_Sx2w[0] = fSx2w;
            inlib::histo::h1d* h = new inlib::histo::h1d("", 10, 0, 1);
            h->copy_from_data(data);
            return h;
        }

        inline bool TH1D_stream_write(buffer& a_buffer,
                                const histo::h1d& a_h,
                                const std::string& a_name)
        {
            if (!a_buffer.write_version(1)) return false;

            if (!TH_write_1D(a_buffer, a_h, a_name, a_h.bins_sum_w2())) return false;

            if (!a_buffer.write_array(a_h.bins_sum_w())) return false; //fArray TArrayD

            return true;
        }

        inline inlib::histo::h2d* TH2F_stream_read(buffer& a_buffer)
        {
            unsigned int s, c;
            short v;

            if (!a_buffer.read_version(v, s, c)) return 0;

            //::printf("debug : BatchLab::Rio::TH2F::stream : version %d\n",v);
            // Version 3 streaming (ROOT/v3-00-6).
            // Now we have to reconstruct a valid Histogram from a_buffer :
            hd_data data;
            data.m_dimension = 2;
            //data.m_coords.resize(data.m_dimension,0);
            //data.m_ints.resize(data.m_dimension,0);
            data.m_axes.resize(2);
            data.m_in_range_plane_Sxyw.resize(1, 0);
            double fEntries; //in range + outflow.
            double fSw;      //in range.
            double fSw2;     //in range.
            double fSxw;     //in range.
            double fSx2w;    //in range.
            double fSyw;     //in range.
            double fSy2w;    //in range.

            if (!TH_read_2D(a_buffer, data, fEntries, fSw, fSw2, fSxw, fSx2w, fSyw, fSy2w)) return 0;

            // the upper set :
            //data.m_title
            //data.m_bin_number
            //data.m_axes
            //data.m_bin_Sw2
            std::vector<float> bins; //fArray TArrayF

            if (!Array_stream<float>(a_buffer, bins)) return 0;

            if (!a_buffer.check_byte_count(s, c, "TH2F")) return 0;

            unsigned int binn = data.m_bin_number;
            //::printf("debug : BatchLab::Rio::TH2F::stream : histo bins %d\n",binn);
            data.m_bin_Sw.resize(binn, 0);
            {
                for (unsigned int index = 0; index < binn; index++) {
                    data.m_bin_Sw[index] = double(bins[index]);
                }
            }
            data.m_bin_entries.resize(binn, 0);
            {
                std::vector<double> empty;
                empty.resize(2, 0);
                data.m_bin_Sxw.resize(binn, empty);
                data.m_bin_Sx2w.resize(binn, empty);
            }
            data.m_all_entries = static_cast<unsigned int>(fEntries);
            data.m_in_range_entries = 0;
            data.m_in_range_Sw = fSw;
            data.m_in_range_Sw2 = fSw2;
            data.m_in_range_Sxw.resize(2, 0);
            data.m_in_range_Sx2w.resize(2, 0);
            data.m_in_range_Sxw[0] = fSxw;
            data.m_in_range_Sx2w[0] = fSx2w;
            data.m_in_range_Sxw[1] = fSyw;
            data.m_in_range_Sx2w[1] = fSy2w;
            inlib::histo::h2d* h = new inlib::histo::h2d("", 10, 0, 1, 10, 0, 1);
            h->copy_from_data(data);
            return h;
        }

        inline bool TH2F_stream_write(buffer& a_buffer,
                                const histo::h2d& a_h,
                                const std::string& a_name)
        {
            if (!a_buffer.write_version(3)) return false;

            if (!TH_write_2D(a_buffer, a_h, a_name, a_h.bins_sum_w2())) return false;

            if (!a_buffer.write_array(convert<double, float>(a_h.bins_sum_w()))) return false; //fArray TArrayF

            return true;
        }

        inline bool TH2F_stream_write(buffer& a_buffer,
                                const histo::h2df& a_h,
                                const std::string& a_name)
        {
            if (!a_buffer.write_version(3)) return false;

            if (!TH_write_2D(a_buffer, a_h, a_name, convert<float, double>(a_h.bins_sum_w2()))) return false;

            if (!a_buffer.write_array(a_h.bins_sum_w())) return false;

            return true;
        }

        inline inlib::histo::h2d* TH2D_stream_read(buffer& a_buffer)
        {
            unsigned int s, c;
            short v;

            if (!a_buffer.read_version(v, s, c)) return 0;

            //::printf("debug : BatchLab::Rio::TH2D::stream : version %d\n",v);
            // Version 3 streaming (ROOT/v3-00-6).
            // Now we have to reconstruct a valid Histogram from a_buffer :
            hd_data data;
            data.m_dimension = 2;
            //data.m_coords.resize(data.m_dimension,0);
            //data.m_ints.resize(data.m_dimension,0);
            data.m_axes.resize(2);
            data.m_in_range_plane_Sxyw.resize(1, 0);
            double fEntries; //in range + outflow.
            double fSw;      //in range.
            double fSw2;     //in range.
            double fSxw;     //in range.
            double fSx2w;    //in range.
            double fSyw;     //in range.
            double fSy2w;    //in range.

            if (!TH_read_2D(a_buffer, data, fEntries, fSw, fSw2, fSxw, fSx2w, fSyw, fSy2w)) return 0;

            // the upper set :
            //data.m_title
            //data.m_bin_number
            //data.m_axes
            //data.m_bin_Sw2
            std::vector<double> bins; //fArray TArrayD

            if (!Array_stream<double>(a_buffer, bins)) return 0;

            if (!a_buffer.check_byte_count(s, c, "TH2D")) return 0;

            unsigned int binn = data.m_bin_number;
            data.m_bin_Sw = bins;
            data.m_bin_entries.resize(binn, 0);
            {
                std::vector<double> empty;
                empty.resize(2, 0);
                data.m_bin_Sxw.resize(binn, empty);
                data.m_bin_Sx2w.resize(binn, empty);
            }
            data.m_all_entries = static_cast<unsigned int>(fEntries);
            data.m_in_range_entries = 0;
            data.m_in_range_Sw = fSw;
            data.m_in_range_Sw2 = fSw2;
            data.m_in_range_Sxw.resize(2, 0);
            data.m_in_range_Sx2w.resize(2, 0);
            data.m_in_range_Sxw[0] = fSxw;
            data.m_in_range_Sx2w[0] = fSx2w;
            data.m_in_range_Sxw[1] = fSyw;
            data.m_in_range_Sx2w[1] = fSy2w;
            inlib::histo::h2d* h = new inlib::histo::h2d("", 10, 0, 1, 10, 0, 1);
            h->copy_from_data(data);
            return h;
        }

        inline bool TH2D_stream_write(buffer& a_buffer,
                                const histo::h2d& a_h,
                                const std::string& a_name)
        {
            if (!a_buffer.write_version(3)) return false;

            if (!TH_write_2D(a_buffer, a_h, a_name, a_h.bins_sum_w2())) return false;

            if (!a_buffer.write_array(a_h.bins_sum_w())) return false; //fArray TArrayD

            return true;
        }

        inline inlib::histo::h3d* TH3D_stream_read(buffer& a_buffer)
        {
            unsigned int s, c;
            short v;

            if (!a_buffer.read_version(v, s, c)) return 0;

            //::printf("debug : BatchLab::Rio::TH2D::stream : version %d\n",v);
            // Now we have to reconstruct a valid Histogram from a_buffer :
            hd_data data;
            data.m_dimension = 3;
            //data.m_coords.resize(data.m_dimension,0);
            //data.m_ints.resize(data.m_dimension,0);
            data.m_axes.resize(2);
            data.m_in_range_plane_Sxyw.resize(3, 0);
            double fEntries; //in range + outflow.
            double fSw;      //in range.
            double fSw2;     //in range.
            double fSxw;     //in range.
            double fSx2w;    //in range.
            double fSyw;     //in range.
            double fSy2w;    //in range.
            double fSzw;     //in range.
            double fSz2w;    //in range.

            if (!TH_read_3D(a_buffer, data, fEntries, fSw, fSw2, fSxw, fSx2w, fSyw, fSy2w, fSzw, fSz2w)) return 0;

            // the upper set :
            //data.m_title
            //data.m_bin_number
            //data.m_axes
            //data.m_bin_Sw2
            std::vector<double> bins; //fArray TArrayD

            if (!Array_stream<double>(a_buffer, bins)) return 0;

            if (!a_buffer.check_byte_count(s, c, "TH3D")) return 0;

            unsigned int binn = data.m_bin_number;
            data.m_bin_Sw = bins;
            data.m_bin_entries.resize(binn, 0);
            {
                std::vector<double> empty;
                empty.resize(3, 0);
                data.m_bin_Sxw.resize(binn, empty);
                data.m_bin_Sx2w.resize(binn, empty);
            }
            data.m_all_entries = static_cast<unsigned int>(fEntries);
            data.m_in_range_entries = 0;
            data.m_in_range_Sw = fSw;
            data.m_in_range_Sw2 = fSw2;
            data.m_in_range_Sxw.resize(3, 0);
            data.m_in_range_Sx2w.resize(3, 0);
            data.m_in_range_Sxw[0] = fSxw;
            data.m_in_range_Sx2w[0] = fSx2w;
            data.m_in_range_Sxw[1] = fSyw;
            data.m_in_range_Sx2w[1] = fSy2w;
            data.m_in_range_Sxw[2] = fSzw;
            data.m_in_range_Sx2w[2] = fSz2w;
            inlib::histo::h3d* h = new inlib::histo::h3d("", 10, 0, 1, 10, 0, 1, 10, 0, 1);
            h->copy_from_data(data);
            return h;
        }

        inline bool TH3F_stream_write(buffer& a_buffer, const histo::h3d& a_h, const std::string& a_name)
        {
            if (!a_buffer.write_version(3)) return false;

            if (!TH_write_3D(a_buffer, a_h, a_name, a_h.bins_sum_w2())) return false;

            if (!a_buffer.write_array(convert<double, float>(a_h.bins_sum_w()))) return false; //fArray TArrayF

            return true;
        }

        inline bool TH3F_stream_write(buffer& a_buffer, const histo::h3df& a_h, const std::string& a_name)
        {
            if (!a_buffer.write_version(3)) return false;

            if (!TH_write_3D(a_buffer, a_h, a_name, convert<float, double>(a_h.bins_sum_w2()))) return false;

            if (!a_buffer.write_array(a_h.bins_sum_w())) return false; //fArray TArrayF

            return true;
        }

        inline bool TH3D_stream_write(buffer& a_buffer, const histo::h3d& a_h, const std::string& a_name)
        {
            if (!a_buffer.write_version(3)) return false;

            if (!TH_write_3D(a_buffer, a_h, a_name, a_h.bins_sum_w2())) return false;

            if (!a_buffer.write_array(a_h.bins_sum_w())) return false; //fArray TArrayD

            return true;
        }

        inline inlib::histo::p1d* TProfile_stream_read(buffer& a_buffer)
        {
            unsigned int s, c;
            short v;

            if (!a_buffer.read_version(v, s, c)) return 0;

            // Version 3 streaming (ROOT/v3-00-6).
            //WARNING : the mapping inlib::histo::p1d / TProfile is not obvious.
            //p1d::m_bin_Svw  <---> TProfile::fArray
            //p1d::m_bin_Sv2w <---> TProfile::fSumw2
            //p1d::m_bin_Sw   <---> TProfile::fBinEntries
            inlib::histo::h1d* h = TH1D_stream(a_buffer);

            if (!h) return 0;

            //NOTE : histo.m_bin_Sw <---> TH1D::TArrayD::fArray
            pd_data data(h->dac());
            delete h;
            std::vector<double> bins; //fBinEntries TArrayD

            if (!Array_stream<double>(a_buffer, bins)) return 0;

            int errorMode;

            if (!a_buffer.read(errorMode)) return 0;

            double ymin;

            if (!a_buffer.read(ymin)) return 0;

            double ymax;

            if (!a_buffer.read(ymax)) return 0;

            if (v >= 4) {
                double  sumwy;

                if (!a_buffer.read(sumwy)) return 0;

                double   sumwy2;

                if (!a_buffer.read(sumwy2)) return 0;
            }

            if (v >= 5) {
                std::vector<double> bins_sumw2; //fBinSumw2 TArrayD

                if (!Array_stream<double>(a_buffer, bins_sumw2)) return 0;
            }

            if (!a_buffer.check_byte_count(s, c, "TProfile")) return 0;

            data.m_is_profile = true;
            data.m_cut_v = true;
            data.m_min_v = ymin;
            data.m_max_v = ymax;
            unsigned int binn = data.m_bin_number;
            data.m_bin_Svw.resize(binn);
            data.m_bin_Sv2w.resize(binn);

            for (unsigned int index = 0; index < binn; index++) {
                double svw = data.m_bin_Sw[index];
                double sv2w = data.m_bin_Sw2[index];
                double sw = bins[index];
                //data.m_bin_entries[index] = (int)sw; //FIXME : ok for w = 1 only !
                data.m_bin_Sw[index] = (double)sw;
                //FIXME : data.m_bin_Sxw
                //FIXME : data.m_bin_Sx2w
                data.m_bin_Svw[index] = svw;
                data.m_bin_Sv2w[index] = sv2w;
            }

            inlib::histo::p1d* p = new inlib::histo::p1d("", 10, 0, 1);
            p->copy_from_data(data);
            // We have now a valid inlib::histo::p1d.
            return p;
        }

        inline bool TProfile_stream_write(buffer& a_buffer, const histo::p1d& a_p, const std::string& a_name)
        {
            if (!a_buffer.write_version(4)) return false;

            //WARNING : the mapping histo::p1d / TProfile is not obvious.
            //p1d::m_bin_Svw  <---> TProfile::fArray
            //p1d::m_bin_Sv2w <---> TProfile::fSumw2
            //p1d::m_bin_Sw   <---> TProfile::fBinEntries

            // TH1D_stream(a_buffer,h,a_name) :
            //if(!a_buffer.write_version(1)) return false;
            //if(!TH_write_1D(a_buffer,a_h,a_name,a_h.bins_sum_w2())) return false; //fSumw2 TArrayD
            //if(!a_buffer.write_array(a_h.bins_sum_w())) return false; //fArray TArrayD
            // but for profile :
            if (!a_buffer.write_version(1)) return false;

            if (!TH_write_1D(a_buffer, a_p, a_name, a_p.bins_sum_v2w())) return false; //fSumw2 TArrayD

            if (!a_buffer.write_array(a_p.bins_sum_vw())) return false; //fArray TArrayD

            //TProfile specific :
            if (!a_buffer.write_array(a_p.bins_sum_w())) return false; //fBinEntries TArrayD

            int errorMode = 0;

            if (!a_buffer.write(errorMode)) return false;

            if (!a_buffer.write(a_p.min_v())) return false;

            if (!a_buffer.write(a_p.max_v())) return false;

            // version 4 :
            if (!a_buffer.write(a_p.get_Svw())) return false;             //Double_t fTsumwy;  //Total Sum of weight*Y

            if (!a_buffer.write(a_p.get_Sv2w())) return false;            //Double_t fTsumwy2; //Total Sum of weight*Y*Y

            return true;
        }

        inline inlib::histo::p2d* TProfile2D_stream_read(buffer& a_buffer)
        {
            unsigned int s, c;
            short v;

            if (!a_buffer.read_version(v, s, c)) return 0;

            // Version 3 streaming (ROOT/v3-00-6).
            //WARNING : the mapping inlib::histo::p1d / TProfile is not obvious.
            //p1d::m_bin_Svw  <---> TProfile::fArray
            //p1d::m_bin_Sv2w <---> TProfile::fSumw2
            //p1d::m_bin_Sw   <---> TProfile::fBinEntries
            inlib::histo::h2d* h = TH2D_stream(a_buffer);

            if (!h) return 0;

            //NOTE : histo.m_bin_Sw <---> TH2D::TArrayD::fArray
            pd_data data(h->dac());
            delete h;
            std::vector<double> bins; //fBinEntries TArrayD

            if (!Array_stream<double>(a_buffer, bins)) return 0;

            int errorMode;

            if (!a_buffer.read(errorMode)) return 0;

            double zmin;

            if (!a_buffer.read(zmin)) return 0;

            double zmax;

            if (!a_buffer.read(zmax)) return 0;

            if (v >= 5) {
                double  sumwz;

                if (!a_buffer.read(sumwz)) return 0;

                double   sumwz2;

                if (!a_buffer.read(sumwz2)) return 0;
            }

            if (v >= 7) {
                std::vector<double> bins_sumw2; //fBinSumw2 TArrayD

                if (!Array_stream<double>(a_buffer, bins_sumw2)) return 0;
            }

            if (!a_buffer.check_byte_count(s, c, "TProfile2D")) return 0;

            data.m_is_profile = true;
            data.m_cut_v = true;
            data.m_min_v = zmin;
            data.m_max_v = zmax;
            unsigned int binn = data.m_bin_number;
            data.m_bin_Svw.resize(binn);
            data.m_bin_Sv2w.resize(binn);

            for (unsigned int index = 0; index < binn; index++) {
                double svw = data.m_bin_Sw[index];
                double sv2w = data.m_bin_Sw2[index];
                double sw = bins[index];
                //data.m_bin_entries[index] = (int)sw; //FIXME : ok for w = 1 only !
                data.m_bin_Sw[index] = (double)sw;
                //FIXME : data.m_bin_Sxw
                //FIXME : data.m_bin_Sx2w
                data.m_bin_Svw[index] = svw;
                data.m_bin_Sv2w[index] = sv2w;
            }

            inlib::histo::p2d* p = new inlib::histo::p2d("", 10, 0, 1, 10, 0, 1);
            p->copy_from_data(data);
            return p;
        }

        inline bool TProfile2D_stream_write(buffer& a_buffer, const histo::p2d& a_p, const std::string& a_name)
        {
            if (!a_buffer.write_version(5)) return false;

            //WARNING : the mapping histo::p2d / TProfile2D is not obvious.
            //p2d::m_bin_Svw  <---> TProfile2D::fArray
            //p2d::m_bin_Sv2w <---> TProfile2D::fSumw2
            //p2d::m_bin_Sw   <---> TProfile2D::fBinEntries

            // TH2D_stream(a_buffer,h,a_name) :
            //if(!a_buffer.write_version(3)) return false;
            //if(!TH_write_2D(a_buffer,a_h,a_name,a_h.bins_sum_w2())) return false; //fSumw2 TArrayD
            //if(!a_buffer.write_array(a_h.bins_sum_w())) return false; //fArray TArrayD
            // for profile :
            if (!a_buffer.write_version(3)) return false;

            if (!TH_write_2D(a_buffer, a_p, a_name, a_p.bins_sum_v2w())) return false; //fSumw2 TArrayD

            if (!a_buffer.write_array(a_p.bins_sum_vw())) return false; //fArray TArrayD

            //TProfile2D specific :
            if (!a_buffer.write_array(a_p.bins_sum_w())) return false; //fBinEntries TArrayD

            int errorMode = 0;

            if (!a_buffer.write(errorMode)) return false;   //fErrorMode

            if (!a_buffer.write(a_p.min_v())) return false; //fZmin

            if (!a_buffer.write(a_p.max_v())) return false; //fZmax

            // version 5 :
            if (!a_buffer.write(a_p.get_Svw())) return false;             //Double_t fTsumwz;  //Total Sum of weight*Z

            if (!a_buffer.write(a_p.get_Sv2w())) return false;            //Double_t fTsumwz2; //Total Sum of weight*Z*Z

            return true;
        }


        class TDirectory : public inlib::rroot::directory {
            //public:
            //  static const std::string& store_class() {return TDirectory_cls();}
        public:
            TDirectory(ifile& a_file)
                : inlib::rroot::directory(a_file)
            {}
            virtual ~TDirectory() {}
        protected:
            TDirectory(const TDirectory& a_from)
                : inlib::rroot::directory(a_from)
            {}
            TDirectory& operator=(const TDirectory&)
            {
                return *this;
            }
        public:
            bool stream(buffer& a_buffer)
            {
                initialize();
                short version;

                if (!a_buffer.read_version(version)) return false;

                unsigned int _date;

                if (!a_buffer.read(_date)) return false;

                //m_date_C.setDate(_date);
                if (!a_buffer.read(_date)) return false;

                //m_date_M.setDate(_date);
                if (!a_buffer.read(m_nbytes_keys)) return false;

                if (!a_buffer.read(m_nbytes_name)) return false;

                if (version > 1000) {
                    if (!a_buffer.read(m_seek_directory)) return false;

                    if (!a_buffer.read(m_seek_parent)) return false;

                    if (!a_buffer.read(m_seek_keys)) return false;
                } else {
                    {
                        seek32 i;

                        if (!a_buffer.read(i)) return false;

                        m_seek_directory = i;
                    }
                    {
                        seek32 i;

                        if (!a_buffer.read(i)) return false;

                        m_seek_parent = i;
                    }
                    {
                        seek32 i;

                        if (!a_buffer.read(i)) return false;

                        m_seek_keys = i;
                    }
                }

                //short v = version%1000;

                if (m_seek_keys) {
                    uint32 n;

                    if (!read_keys(n)) {
                        a_buffer.out() << "inlib::rroot::TDirectory::stream :"
                                       << " cannot read keys."
                                       << std::endl;
                        return false;
                    }
                }

                return true;
            }
        protected:
            void initialize()
            {
                // Initialise directory to defaults :
                // If directory is created via default ctor (when dir is read from file)
                // don't add it here to the directory since its name is not yet known.
                // It will be added to the directory in TKey::ReadObj().
                m_date_C = 0;//m_date_C.set();
                m_date_M = 0;//m_date_M.set();
                m_nbytes_keys = 0;
                m_seek_directory = 0;
                m_seek_parent = 0;
                m_seek_keys = 0;
            }
        };


        inline void dump(std::ostream& a_out,
                         inlib::rroot::ifile& a_file,
                         const std::vector<inlib::rroot::key*>& a_keys,
                         bool a_recursive,
                         unsigned int a_spaces = 0)
        {
            // dump non directory objects :
            {
                std::vector<inlib::rroot::key*>::const_iterator it;

                for (it = a_keys.begin(); it != a_keys.end(); ++it) {
                    inlib::rroot::key& k = *(*it);

                    if (k.object_class() == inlib::rroot::TDirectory_cls()) continue;

                    {
                        for (unsigned int index = 0; index < a_spaces; index++) a_out << " ";
                    }
                    k.dump(a_out);
                }
            }
            // dump directories :
            {
                std::vector<inlib::rroot::key*>::const_iterator it;

                for (it = a_keys.begin(); it != a_keys.end(); ++it) {
                    inlib::rroot::key& k = *(*it);

                    if (k.object_class() != inlib::rroot::TDirectory_cls()) continue;

                    std::string label = k.object_name();
                    {
                        for (unsigned int index = 0; index < a_spaces; index++) a_out << " ";
                    }
                    a_out << "directory : " << label << std::endl;

                    if (!a_recursive) continue;

                    uint32 sz;
                    char* buf = k.get_object_buffer(sz);

                    //we don't have ownership of buf.
                    if (!buf) {
                        a_out  << "inlib::rroot::dump :"
                               << " can't get directory data buffer."
                               << std::endl;
                    } else {
                        inlib::rroot::buffer b(a_out, a_file.byte_swap(),
                                               sz, buf, k.key_length(), false);
                        inlib::rroot::TDirectory tdir(a_file);

                        if (!tdir.stream(b)) {
                            a_out  << "inlib::rroot::dump :"
                                   << " can't stream TDirectory."
                                   << std::endl;
                        } else {
                            const std::vector<inlib::rroot::key*>& keys = tdir.keys();
                            dump(a_out, a_file, keys, a_recursive, a_spaces + 1);
                        }
                    }
                }
            }
        }

        inline bool read_sinfos(inlib::rroot::ifile& a_file)
        {
            key& k = a_file.sinfos_key();
            std::ostream& out = k.file().out();

            if (k.object_class() != inlib::rroot::TList_cls()) {
                out << "inlib::rroot::read_sinfos :"
                    << " key not a TList."
                    << std::endl;
                return false;
            }

            unsigned int sz;
            char* buf = k.get_object_buffer(sz); //we don't have ownership of buf.

            if (!buf) {
                out << "inlib::rroot::read_sinfos :"
                    << " can't get data buffer of " << k.object_name() << "."
                    << std::endl;
                return false;
            }

            inlib::rroot::buffer b(out, k.file().byte_swap(), sz, buf, k.key_length(), false);
            dummy_fac fac(out);
            List infos(fac, true);

            if (!infos.stream(b)) return false;

            inlib_vforcit(iro*, infos, it) {
                StreamerInfo* info = inlib::safe_cast<iro, StreamerInfo>(*(*it));

                if (!info) return false;

                info->out(out);
            }
            return true;
        }


    }
}

#endif
