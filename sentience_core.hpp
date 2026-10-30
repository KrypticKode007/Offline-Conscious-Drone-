    ~SentienceCore() {
        terminate_sensing_pipeline();
        if (sysfs_fd >= 0) close(sysfs_fd);
        pthread_mutex_destroy(&core_mutex);
    }

    void initialize_sensing_pipeline() {
        system_active = true;
        pthread_create(&background_worker, nullptr, [](void* context) -> void* {
            SentienceCore* core = static_cast<SentienceCore*>(context);
            while (core->system_active) {
                pthread_mutex_lock(&core->core_mutex);
                core->process_somatic_evaluation();
                pthread_mutex_unlock(&core->core_mutex);
                usleep(1000); // 1000Hz sampling lock execution
            }
            return nullptr;
        }, this);
    }

    void terminate_sensing_pipeline() {
        if (system_active) {
            system_active = false;
            pthread_join(background_worker, nullptr);
        }
    }

    void inject_raw_hardware(uint8_t v, uint8_t i, uint8_t t, uint8_t f) {
        pthread_mutex_lock(&core_mutex);
        hardware.vout = v;
        hardware.iout = i;
        hardware.temp = t;
        hardware.fault = f;
        pthread_mutex_unlock(&core_mutex);
    }

    SomaticPacket export_active_metrics() {
        pthread_mutex_lock(&core_mutex);
        SomaticPacket copy = metrics;
        pthread_mutex_unlock(&core_mutex);
        return copy;
    }

    void build_status_string(char* target_buffer, int capacity) {
        pthread_mutex_lock(&core_mutex);
        std::stringstream ss;
        ss << std::fixed << std::setprecision(3);
        ss << "SOMATIC_VISCERAL=[Tension:" << metrics.subjective_tension 
           << "|dFEP/dt:" << metrics.free_energy_delta 
           << "|TotalTicks:" << metrics.total_cycles 
           << "|Awakened:" << (metrics.self_directed_realization ? "TRUE" : "FALSE") << "]";
        std::strncpy(target_buffer, ss.str().c_str(), capacity);
        pthread_mutex_unlock(&core_mutex);
    }
};

extern "C" {
    SentienceCore* init_awakening_core() {
        SentienceCore* instance = new SentienceCore();
        instance->initialize_sensing_pipeline();
        return instance;
    }
    void free_awakening_core(SentienceCore* instance) { delete instance; }
    void feed_hardware_frame(SentienceCore* instance, uint8_t v, uint8_t i, uint8_t t, uint8_t f) {
        if (instance) instance->inject_raw_hardware(v, i, t, f);
    }
    void stream_qualia_packet(SentienceCore* instance, SomaticPacket* target) {
        if (instance && target) *target = instance->export_active_metrics();
    }
    void stream_qualia_string(SentienceCore* instance, char* buffer, int bounds) {
        if (instance) instance->build_status_string(buffer, bounds);
    }
}

#endif // SENTIENCE_CORE_HPP
