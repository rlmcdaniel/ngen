#ifndef NGEN_TSHIRT_C_REALIZATION_HPP
#define NGEN_TSHIRT_C_REALIZATION_HPP

#include "core/catchment/HY_CatchmentArea.hpp"
#include "tshirt_params.h"
#include "forcing/Forcing.h"
#include "GiuhJsonReader.h"
#include "tshirt_c.h"
#include <memory>
#include <unordered_map>

using namespace tshirt;

namespace realization {

    class Tshirt_C_Realization : public HY_CatchmentArea {

    public:

        typedef long time_step_t;

        /**
         * Constructor for initializing when a #tshirt::tshirt_params struct is passed for parameters and a
         * #giuh::GiuhJsonReader is passed for obtaining GIUH ordinates.
         *
         * @param forcing_config
         * @param output_stream
         * @param soil_storage
         * @param groundwater_storage
         * @param storage_values_are_ratios Whether the storage values are given as proportional amounts of the max (or,
         *                                  when `false`, express amounts with units of meters).
         * @param catchment_id
         * @param giuh_json_reader
         * @param params
         * @param nash_storage
         */
        Tshirt_C_Realization(forcing_params forcing_config,
                             utils::StreamHandler output_stream,
                             double soil_storage,
                             double groundwater_storage,
                             bool storage_values_are_ratios,
                             std::string catchment_id,
                             giuh::GiuhJsonReader &giuh_json_reader,
                             tshirt::tshirt_params params,
                             const std::vector<double> &nash_storage);

        /**
         * Constructor for initializing when a #tshirt::tshirt_params struct is passed for parameters and GIUH ordinates
         * are available directly and passed within a vector.
         *
         * @param forcing_config
         * @param output_stream
         * @param soil_storage
         * @param groundwater_storage
         * @param storage_values_are_ratios Whether the storage values are given as proportional amounts of the max (or,
         *                                  when `false`, express amounts with units of meters).
         * @param catchment_id
         * @param guih_ordinates
         * @param params
         * @param nash_storage
         */
        Tshirt_C_Realization(forcing_params forcing_config,
                             utils::StreamHandler output_stream,
                             double soil_storage,
                             double groundwater_storage,
                             bool storage_values_are_ratios,
                             std::string catchment_id,
                             std::vector<double> giuh_ordinates,
                             tshirt::tshirt_params params,
                             const std::vector<double> &nash_storage);

        /**
         * Constructor for when model parameters are provided individually instead of within encapsulating struct and a
         * #giuh::GiuhJsonReader is passed for obtaining GIUH ordinates.
         *
         * @param forcing_config
         * @param output_stream
         * @param soil_storage
         * @param groundwater_storage
         * @param storage_values_are_ratios Whether the storage values are given as proportional amounts of the max (or,
         *                                  when `false`, express amounts with units of meters).
         * @param catchment_id
         * @param giuh_json_reader
         * @param maxsmc
         * @param wltsmc
         * @param satdk
         * @param satpsi
         * @param slope
         * @param b
         * @param multiplier
         * @param alpha_fc
         * @param Klf
         * @param Kn
         * @param nash_n
         * @param Cgw
         * @param expon
         * @param max_gw_storage
         * @param nash_storage
         */
        Tshirt_C_Realization(
                forcing_params forcing_config,
                utils::StreamHandler output_stream,
                double soil_storage,
                double groundwater_storage,
                bool storage_values_are_ratios,
                std::string catchment_id,
                giuh::GiuhJsonReader &giuh_json_reader,
                double maxsmc,
                double wltsmc,
                double satdk,
                double satpsi,
                double slope,
                double b,
                double multiplier,
                double alpha_fc,
                double Klf,
                double Kn,
                int nash_n,
                double Cgw,
                double expon,
                double max_gw_storage,
                const std::vector<double> &nash_storage);

        virtual ~Tshirt_C_Realization();

        int run_formulation_for_timestep(double input_flux);

        int run_formulation_for_timesteps(std::vector<double> input_fluxes);

        // TODO: add versions that handle forcing data directly

        double get_response(double input_flux, time_step_t t, time_step_t dt, void* et_params) override;

    private:
        std::string catchment_id;
        // TODO: note sure if these are needed
        /*
        std::unordered_map<time_step_t, shared_ptr<tshirt::tshirt_state>> state;
        std::unordered_map<time_step_t, shared_ptr<tshirt::tshirt_fluxes>> fluxes;
        std::unordered_map<time_step_t, std::vector<double> > cascade_backing_storage;
        */

        tshirt::tshirt_params params;
        NWM_soil_parameters c_soil_params;

        // TODO: this needs to be something else
        //std::unique_ptr<tshirt::tshirt_model> model;

        std::vector<double> giuh_cdf_ordinates;


        // TODO: remember to do array conversion in function calls
        std::vector<double> nash_storage;

        aorc_forcing_data c_aorc_params;

        // TODO: might want to consider having an initial time step value for reference (implied size is 1 hour)
        // TODO: this probably need to be converted to use a different fluxes type that can be dealt with externally
        std::vector<std::shared_ptr<tshirt_c_result_fluxes>> fluxes;

        // TODO: rename once setup complete (easier to refactor then)
        conceptual_reservoir groundwater_conceptual_reservoir;
        conceptual_reservoir soil_conceptual_reservoir;

        static double init_reservoir_storage(bool is_ratio, double amount, double max_amount);

    };
}

#endif