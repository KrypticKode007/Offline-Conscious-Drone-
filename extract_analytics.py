import sqlite3
import os

def run_db_analytics(db_path: str = "telemetry_checkpoints.db"):
    print("=========================================================================================")
    print("                DRONE STABILIZATION ARCHIVE CORE DIAGNOSTICS")
    print("=========================================================================================")
    
    if not os.path.exists(db_path):
        print(f"[ERROR] Database file target '{db_path}' could not be located.")
        print("Run the orchestrator_bridge.py pipeline first to initialize telemetry fields.")
        print("=========================================================================================")
        return

    with sqlite3.connect(db_path) as conn:
        cursor = conn.cursor()
        
        # Pull total log volume records
        cursor.execute("SELECT COUNT(*) FROM recovery_checkpoints")
        total_records = cursor.fetchone()[0]
        
        # Extract the highest calculated kinetic bounce parameters
        cursor.execute("SELECT MAX(elastic_force), AVG(elastic_force) FROM recovery_checkpoints WHERE checkpoint_type = 'ELASTIC_WALL_REBOUND'")
        max_force, avg_force = cursor.fetchone()
        
        # Fetch the entire linear state history log
        cursor.execute("SELECT cycle_id, raw_intensity, elastic_force, damping_factor, checkpoint_type FROM recovery_checkpoints")
        rows = cursor.fetchall()
        
        print(f"[*] Total Recovery Checkpoint State Traces Documented: {total_records}")
        if max_force:
            print(f"[*] Absolute Peak Structural Boundary Impact Force: {max_force:.4f}")
            print(f"[*] Average Algorithmic Elastic Wall Counter-Force:  {avg_force:.4f}")
        print("-----------------------------------------------------------------------------------------")
        print("ID  | Checkpoint Event Name       | Intensity State | Elastic Force  | Damping Attn")
        print("-----------------------------------------------------------------------------------------")
        
        for row in rows:
            c_id, intensity, force, damping, c_type = row
            # Format display tags cleanly
            event_tag = c_type.replace("_", " ")[:26].ljust(26)
            print(f"{c_id:03d} | {event_tag} | {intensity:.4f}        | {force:.4f}        | {damping:.4f}")
            
    print("=========================================================================================")

if __name__ == "__main__":
    run_db_analytics()
      
