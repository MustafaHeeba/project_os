import tkinter as tk
from tkinter import ttk, messagebox
import matplotlib.pyplot as plt

class SJFApp:
    def __init__(self, root):
        self.root = root
        self.root.title("Shortest Job First (SJF) Scheduler - GUI")
        self.root.geometry("400x250")
        self.root.config(bg="#f4f4f4")
        self.entries = []
        self.create_start_screen()

    def create_start_screen(self):
        self.clear_window()

        title = tk.Label(self.root, text="Welcome to SJF Scheduler", font=("Helvetica", 16, "bold"), bg="#f4f4f4", fg="#333")
        title.pack(pady=20)

        label = tk.Label(self.root, text="Enter number of processes:", font=("Helvetica", 12), bg="#f4f4f4")
        label.pack()

        self.num_entry = tk.Entry(self.root, font=("Helvetica", 12), justify="center")
        self.num_entry.pack(pady=10)

        start_btn = tk.Button(self.root, text="Start", command=self.start_input_ui, bg="#4caf50", fg="white", font=("Helvetica", 12, "bold"))
        start_btn.pack(pady=10)

    def clear_window(self):
        for widget in self.root.winfo_children():
            widget.destroy()

    def start_input_ui(self):
        try:
            self.num_processes = int(self.num_entry.get())
            if self.num_processes <= 0:
                raise ValueError
        except ValueError:
            messagebox.showerror("Input Error", "Please enter a positive number.")
            return

        self.clear_window()
        self.root.geometry("850x600")
        self.entries = []

        title = tk.Label(self.root, text="SJF (Non-Preemptive) Scheduling", font=("Helvetica", 18, "bold"), bg="#f4f4f4", fg="#333")
        title.pack(pady=10)

        input_frame = tk.Frame(self.root, bg="#f4f4f4")
        input_frame.pack(pady=10)

        tk.Label(input_frame, text="Process ID", font=("Helvetica", 10, "bold"), bg="#f4f4f4").grid(row=0, column=0, padx=10)
        tk.Label(input_frame, text="Arrival Time", font=("Helvetica", 10, "bold"), bg="#f4f4f4").grid(row=0, column=1, padx=10)
        tk.Label(input_frame, text="Burst Time", font=("Helvetica", 10, "bold"), bg="#f4f4f4").grid(row=0, column=2, padx=10)

        for i in range(self.num_processes):
            pid_label = tk.Label(input_frame, text=f"P{i+1}", bg="#f4f4f4", font=("Helvetica", 10))
            pid_label.grid(row=i+1, column=0)
            
            at_entry = tk.Entry(input_frame, width=10)
            bt_entry = tk.Entry(input_frame, width=10)
            at_entry.grid(row=i+1, column=1, padx=5, pady=5)
            bt_entry.grid(row=i+1, column=2, padx=5, pady=5)
            
            self.entries.append((pid_label, at_entry, bt_entry))

        simulate_btn = tk.Button(self.root, text="▶️ Run SJF", command=self.run_sjf, bg="#2196f3", fg="white", font=("Helvetica", 12, "bold"))
        simulate_btn.pack(pady=10)

        self.result_table = ttk.Treeview(self.root, columns=("PID", "WT", "TAT", "RT"), show='headings', height=5)
        self.result_table.heading("PID", text="Process")
        self.result_table.heading("WT", text="Waiting Time")
        self.result_table.heading("TAT", text="Turnaround Time")
        self.result_table.heading("RT", text="Response Time")
        self.result_table.pack(pady=10)

        self.avg_label = tk.Label(self.root, text="", font=("Helvetica", 11, "bold"), bg="#f4f4f4", fg="#444")
        self.avg_label.pack()

    def run_sjf(self):
        try:
            processes = []
            for i, (pid_label, at_entry, bt_entry) in enumerate(self.entries):
                at = int(at_entry.get())
                bt = int(bt_entry.get())
                if at < 0 or bt <= 0:
                    raise ValueError("Invalid input values.")
                processes.append({'pid': f'P{i+1}', 'at': at, 'bt': bt})

        except ValueError:
            messagebox.showerror("Input Error", "Please enter valid positive integers only.")
            return

        processes.sort(key=lambda x: (x['at'], x['bt']))

        time = 0
        completed = []
        waiting_times = []
        turnaround_times = []
        response_times = []
        gantt_chart = []

        while processes:
            available = [p for p in processes if p['at'] <= time]
            if not available:
                time = processes[0]['at']
                available = [p for p in processes if p['at'] <= time]

            current = min(available, key=lambda x: x['bt'])
            processes.remove(current)

            start_time = time
            time += current['bt']
            end_time = time

            wt = start_time - current['at']
            tat = end_time - current['at']
            rt = wt

            waiting_times.append(wt)
            turnaround_times.append(tat)
            response_times.append(rt)

            completed.append((current['pid'], wt, tat, rt))
            gantt_chart.append((current['pid'], start_time, end_time))

        for row in self.result_table.get_children():
            self.result_table.delete(row)
        for row in completed:
            self.result_table.insert("", "end", values=row)

        n = len(completed)
        avg_wt = sum(waiting_times) / n
        avg_tat = sum(turnaround_times) / n
        avg_rt = sum(response_times) / n

        self.avg_label.config(
            text=f"Average WT: {avg_wt:.2f}    |    Average TAT: {avg_tat:.2f}    |    Average RT: {avg_rt:.2f}"
        )

        self.plot_gantt(gantt_chart)

    def plot_gantt(self, chart):
        plt.rcParams['toolbar'] = 'None'  
        fig, ax = plt.subplots(figsize=(10, 2))
        ax.set_title("Gantt Chart - SJF Scheduling", fontsize=14)
        ax.set_xlabel("Time")
        ax.set_yticks([])
        ax.set_yticklabels([])

        colors = ['#1f77b4', '#ff7f0e', '#2ca02c', '#d62728', '#9467bd', '#8c564b', '#e377c2', '#7f7f7f']

        for i, (pid, start, end) in enumerate(chart):
            ax.barh(0, end - start, left=start, height=0.5, color=colors[i % len(colors)], edgecolor="black")
            ax.text((start + end) / 2, 0, pid, ha='center', va='center', color='white', fontsize=10, fontweight='bold')
            ax.text(start, 0.6, str(start), ha='center', va='bottom', fontsize=9)
            if i == len(chart) - 1:
                ax.text(end, 0.6, str(end), ha='center', va='bottom', fontsize=9)

        ax.set_xlim(left=0)
        ax.set_ylim(-1, 1)
        ax.get_yaxis().set_visible(False)
        ax.spines['top'].set_visible(False)
        ax.spines['right'].set_visible(False)
        ax.spines['left'].set_visible(False)
        ax.spines['bottom'].set_position('zero')
        plt.tight_layout()
        plt.show()


if __name__ == "__main__":
    root = tk.Tk()
    app = SJFApp(root)
    root.mainloop()
