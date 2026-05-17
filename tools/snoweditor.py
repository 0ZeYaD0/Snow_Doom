import tkinter as tk
from tkinter import ttk, filedialog, messagebox
import math
import os
import copy

ZOOM_DEFAULT = 10
CANVAS_WIDTH  = 800
CANVAS_HEIGHT = 600
HANDLE_PX     = 7   # corner handle half-size in screen pixels

class SnowEditor(tk.Tk):
    def __init__(self):
        super().__init__()
        self.title("Snow Doom - Map Editor")
        self.geometry("1280x800")

        self.entities       = []
        self.selected_index = -1

        self.cam_x = 0.0
        self.cam_z = 0.0
        self.zoom  = ZOOM_DEFAULT

        # drag state — mode: None | "move" | "resize"
        self.drag = {"mode": None, "anchor_wx": 0, "anchor_wz": 0, "offset_x": 0, "offset_z": 0}

        self.res_dir            = self._find_res_dir()
        self.available_textures = []
        self.available_models   = []
        self.scan_assets()

        # Mini 3D Preview State
        self.preview_rot_y = 0.0
        self.preview_rot_x = 0.5  # Pitch down slightly so we aren't at ground level

        self.setup_ui()
        self.draw_canvas()

        # Binds
        self.bind("<Control-d>", self.duplicate_object)
        self.bind("<Control-D>", self.duplicate_object)

    # ------------------------------------------------------------------ #
    #  ASSET DISCOVERY                                                   #
    # ------------------------------------------------------------------ #

    def _find_res_dir(self):
        script_dir = os.path.dirname(os.path.abspath(__file__))
        candidates = [
            os.path.join(script_dir, "..", "res"),
            os.path.join(script_dir, "res"),
            os.path.join(os.getcwd(), "..", "res"),
            os.path.join(os.getcwd(), "res"),
        ]
        for p in candidates:
            norm = os.path.normpath(p)
            if os.path.isdir(norm) and os.path.isdir(os.path.join(norm, "texture")):
                print(f"[SnowEditor] res: {norm}")
                return norm
        messagebox.showwarning("Assets Not Found",
                               "Could not find the 'res' folder. Please select it manually.")
        sel = filedialog.askdirectory(title="Select the Snow Doom 'res' folder")
        if sel and os.path.isdir(sel):
            return sel
        return os.path.normpath(os.path.join(script_dir, "..", "res"))

    def scan_assets(self):
        tex_dir = os.path.join(self.res_dir, "texture")
        self.available_textures.clear()
        self.available_models.clear()

        if os.path.isdir(tex_dir):
            for root, _, files in os.walk(tex_dir):
                for f in sorted(files):
                    if f.lower().endswith((".png", ".jpg", ".jpeg")):
                        # Get relative path from the 'texture' folder root
                        rel = os.path.relpath(os.path.join(root, f), tex_dir)
                        # Standardize to forward slashes for cross-platform engine consistency
                        self.available_textures.append(rel.replace("\\", "/"))
            self.available_textures.sort()
        else:
            self.available_textures = ["Stone/Stone01.png", "Brick/Brick01.png"]

        # Scan specifically inside the texture/mobs directory for enemy images
        mobs_dir = os.path.join(tex_dir, "mobs")
        self.available_models.clear() 

        if os.path.isdir(mobs_dir):
            for root, _, files in os.walk(mobs_dir):
                for f in sorted(files):
                    if f.lower().endswith((".png", ".jpg", ".jpeg")):
                        rel = os.path.relpath(os.path.join(root, f), tex_dir)
                        self.available_models.append(rel.replace("\\", "/"))
            self.available_models.sort()
        else:
            self.available_models = ["mobs/enemy01.png", "mobs/enemy02.png"]

        print(f"[SnowEditor] {len(self.available_textures)} textures, {len(self.available_models)} mob textures")

    def change_res_folder(self):
        sel = filedialog.askdirectory(title="Select the Snow Doom 'res' folder",
                                      initialdir=self.res_dir)
        if not sel:
            return
        if not os.path.isdir(os.path.join(sel, "texture")):
            messagebox.showerror("Wrong Folder",
                                 f"No 'texture' subfolder found in:\n{sel}\n\n"
                                 "Please select the 'res' folder directly.")
            return
        self.res_dir = sel
        self.scan_assets()
        if self.selected_index >= 0:
            self.populate_properties()
        self._status(f"Assets reloaded: {self.res_dir}")

    # ------------------------------------------------------------------ #
    #  UI                                                                  #
    # ------------------------------------------------------------------ #

    def setup_ui(self):
        # ── Canvas ───────────────────────────────────────────────────────
        left = tk.Frame(self)
        left.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)

        self.canvas = tk.Canvas(left, bg="#1a1a1a", width=CANVAS_WIDTH, height=CANVAS_HEIGHT)
        self.canvas.pack(fill=tk.BOTH, expand=True)
        self.canvas.bind("<ButtonPress-1>",   self._click)
        self.canvas.bind("<B1-Motion>",       self._drag_move)
        self.canvas.bind("<ButtonRelease-1>", self._drop)
        self.canvas.bind("<ButtonPress-3>",   self._pan_start)
        self.canvas.bind("<B3-Motion>",       self._pan_drag)
        self.canvas.bind("<Motion>",          self._hover)
        self.canvas.bind("<MouseWheel>",      self._wheel)
        self.canvas.bind("<Button-4>",        self._wheel)
        self.canvas.bind("<Button-5>",        self._wheel)

        # Mini 3D Preview Canvas
        self.preview_canvas = tk.Canvas(self.canvas, width=200, height=200, bg="#1e1e1e", highlightthickness=1, highlightbackground="#444")
        self.preview_canvas.place(x=10, y=30)
        self.preview_canvas.bind("<B1-Motion>", self._rotate_preview)
        self.preview_canvas.bind("<ButtonRelease-1>", self._on_preview_release)

        self.status_var = tk.StringVar(value="Ready")
        tk.Label(left, textvariable=self.status_var, anchor="w",
                 font=("Courier", 9), bg="#111", fg="#aaa",
                 bd=1, relief=tk.SUNKEN).pack(fill=tk.X)

        # ── Right panel ──────────────────────────────────────────────────
        # ── Right panel ──────────────────────────────────────────────────
        # 1. Create a fixed-width container for the canvas and scrollbar
        right_container = tk.Frame(self, width=330)
        right_container.pack(side=tk.RIGHT, fill=tk.Y)
        right_container.pack_propagate(False) # Prevents it from shrinking to fit contents

        # 2. Create the Canvas and Scrollbar
        self.right_canvas = tk.Canvas(right_container, highlightthickness=0)
        self.right_scrollbar = ttk.Scrollbar(right_container, orient="vertical", command=self.right_canvas.yview)
        
        # 3. Create the actual 'right' frame inside the canvas
        right = tk.Frame(self.right_canvas, padx=8, pady=8)
        
        self.right_canvas.configure(yscrollcommand=self.right_scrollbar.set)
        
        self.right_scrollbar.pack(side=tk.RIGHT, fill=tk.Y)
        self.right_canvas.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
        
        # 4. Create a window in the canvas to hold the frame
        self.right_canvas_window = self.right_canvas.create_window((0, 0), window=right, anchor="nw")
        
        # 5. Bind configure events to update scroll region and width dynamically
        def _configure_right_frame(event):
            self.right_canvas.configure(scrollregion=self.right_canvas.bbox("all"))
        right.bind("<Configure>", _configure_right_frame)
        
        def _configure_right_canvas(event):
            self.right_canvas.itemconfig(self.right_canvas_window, width=event.width)
        self.right_canvas.bind("<Configure>", _configure_right_canvas)

        # 6. Setup cross-platform mouse wheel scrolling logic
        def _on_right_scroll(event):
            # event.delta works for Windows/macOS, event.num handles Linux (X11)
            if event.num == 4 or getattr(event, "delta", 0) > 0:
                self.right_canvas.yview_scroll(-1, "units")
            elif event.num == 5 or getattr(event, "delta", 0) < 0:
                self.right_canvas.yview_scroll(1, "units")

        self.right_canvas.bind("<MouseWheel>", _on_right_scroll)
        self.right_canvas.bind("<Button-4>", _on_right_scroll)
        self.right_canvas.bind("<Button-5>", _on_right_scroll)

        # File
        tk.Label(right, text="File", font=("Arial", 11, "bold")).pack(anchor="w")
        row = tk.Frame(right); row.pack(fill=tk.X, pady=(0, 2))
        tk.Button(row, text="Load .map", command=self.load_map).pack(side=tk.LEFT, fill=tk.X, expand=True, padx=(0,2))
        tk.Button(row, text="Save .map", command=self.save_map).pack(side=tk.LEFT, fill=tk.X, expand=True)
        tk.Button(right, text="Change Assets Folder", command=self.change_res_folder,
                  font=("Arial", 8)).pack(fill=tk.X, pady=(2, 6))

        ttk.Separator(right, orient="horizontal").pack(fill=tk.X, pady=4)

        # ── Place ────────────────────────────────────────────────────────
        tk.Label(right, text="Place", font=("Arial", 11, "bold")).pack(anchor="w")

        tk.Button(right, text="  +  Block  (1 x 4 x 1)",
                  command=lambda: self._add_block(x=self.cam_x, y=0, z=self.cam_z, sx=1, sy=4, sz=1),
                  bg="#2a4a2a", fg="white", relief=tk.FLAT, pady=4
                  ).pack(fill=tk.X, pady=2)

        tk.Button(right, text="  +  Floor  (10 x 1 x 10)  — one slab",
                  command=lambda: self._add_block(x=self.cam_x, y=0, z=self.cam_z, sx=10, sy=1, sz=10),
                  bg="#1e3a50", fg="white", relief=tk.FLAT, pady=3
                  ).pack(fill=tk.X, pady=1)

        wf = tk.Frame(right); wf.pack(fill=tk.X, pady=1)
        tk.Button(wf, text="+  Wall  (X axis)",
                  command=lambda: self._add_block(x=self.cam_x, y=4, z=self.cam_z, sx=10, sy=8, sz=1),
                  bg="#1e3a50", fg="white", relief=tk.FLAT, pady=3
                  ).pack(side=tk.LEFT, fill=tk.X, expand=True, padx=(0, 2))
        tk.Button(wf, text="+  Wall  (Z axis)",
                  command=lambda: self._add_block(x=self.cam_x, y=4, z=self.cam_z, sx=1, sy=8, sz=10),
                  bg="#1e3a50", fg="white", relief=tk.FLAT, pady=3
                  ).pack(side=tk.LEFT, fill=tk.X, expand=True)

        tk.Button(right, text="  +  Door", command=self._add_door,
                  bg="#8b5a2b", fg="white", relief=tk.FLAT, pady=3
                  ).pack(fill=tk.X, pady=(6, 0))
        
        tk.Button(right, text="  +  Pickup", command=self._add_pickup,
                  bg="#2b8b5a", fg="white", relief=tk.FLAT, pady=3
                  ).pack(fill=tk.X, pady=(2, 0))

        tk.Button(right, text="  +  Enemy / Sprite",
                  command=lambda: self._add_enemy(),
                  bg="#4a1a1a", fg="white", relief=tk.FLAT, pady=3
                  ).pack(fill=tk.X, pady=(6, 0))


        ttk.Separator(right, orient="horizontal").pack(fill=tk.X, pady=6)

        # ── Entities list ────────────────────────────────────────────────
        tk.Label(right, text="Entities", font=("Arial", 11, "bold")).pack(anchor="w")
        self.listbox = tk.Listbox(right, height=7, font=("Courier", 8),
                                  bg="#161616", fg="#cccccc", selectbackground="#334")
        self.listbox.pack(fill=tk.X)
        self.listbox.bind("<<ListboxSelect>>", self._listbox_select)
        tk.Button(right, text="Delete Selected", command=self._delete,
                  fg="red").pack(fill=tk.X, pady=(2, 0))

        ttk.Separator(right, orient="horizontal").pack(fill=tk.X, pady=6)

        # ── Properties ───────────────────────────────────────────────────
        tk.Label(right, text="Properties", font=("Arial", 11, "bold")).pack(anchor="w")

        pf = tk.Frame(right); pf.pack(fill=tk.X)
        pf.columnconfigure(2, weight=1)

        self.prop_vars = {}

        # Position rows
        for r, (field, label) in enumerate([("type","Type"),("x","X"),("y","Y"),("z","Z")]):
            tk.Label(pf, text=label, anchor="e", width=7,
                     font=("Arial", 9)).grid(row=r, column=0, sticky="e", padx=(0,4), pady=2)
            var = tk.StringVar()
            self.prop_vars[field] = var
            e = tk.Entry(pf, textvariable=var, font=("Courier", 9))
            e.grid(row=r, column=1, columnspan=3, sticky="ew", pady=2)
            e.bind("<KeyRelease>", self._prop_edit)
            if field == "type":
                e.configure(state="readonly")

        # Separator
        tk.Label(pf, text="Size", font=("Arial", 9, "bold"), fg="#888"
                 ).grid(row=4, column=0, columnspan=4, sticky="w", padx=4, pady=(8,2))

        # Size rows with − / + buttons
        for r, (field, label) in enumerate([("sx","Width"),("sy","Height"),("sz","Depth")], start=5):
            tk.Label(pf, text=label, anchor="e", width=7,
                     font=("Arial", 9)).grid(row=r, column=0, sticky="e", padx=(0,4), pady=2)
            tk.Button(pf, text="-", width=2,
                      command=lambda f=field: self._nudge(f, -1)
                      ).grid(row=r, column=1, padx=(0,1), pady=2)
            var = tk.StringVar()
            self.prop_vars[field] = var
            e = tk.Entry(pf, textvariable=var, font=("Courier", 9))
            e.grid(row=r, column=2, sticky="ew", pady=2)
            e.bind("<KeyRelease>", self._prop_edit)
            tk.Button(pf, text="+", width=2,
                      command=lambda f=field: self._nudge(f, +1)
                      ).grid(row=r, column=3, padx=(1,0), pady=2)

        # TEXTURE TILING TOGGLE
        self.tile_var = tk.IntVar(value=1)
        self.tile_cb = tk.Checkbutton(pf, text="Repeat Texture (Tile) vs Stretch", variable=self.tile_var,
                                      command=self._tile_edit, font=("Arial", 9))
        self.tile_cb.grid(row=9, column=0, columnspan=4, sticky="w", padx=4, pady=(5,0))

        # Logic fields
        tk.Label(pf, text="Logic", font=("Arial", 9, "bold"), fg="#888"
                 ).grid(row=10, column=0, columnspan=4, sticky="w", padx=4, pady=(8,2))

        for r, (field, label) in enumerate([("req_key", "Req Key"), ("pickup_type", "P. Type"), ("item_id", "Item ID")], start=11):
            tk.Label(pf, text=label, anchor="e", width=7,
                     font=("Arial", 9)).grid(row=r, column=0, sticky="e", padx=(0,4), pady=2)
            var = tk.StringVar()
            self.prop_vars[field] = var
            e = tk.Entry(pf, textvariable=var, font=("Courier", 9))
            e.grid(row=r, column=1, columnspan=3, sticky="ew", pady=2)
            e.bind("<KeyRelease>", self._prop_edit)

        # Y quick levels
        yf = tk.Frame(right); yf.pack(fill=tk.X, pady=(6, 2))
        tk.Label(yf, text="Y level:", font=("Arial", 8), fg="#888").pack(side=tk.LEFT)
        for yv in [0, 4, 8, 12, 16]:
            tk.Button(yf, text=str(yv), width=3, font=("Arial", 8),
                      command=lambda v=yv: self._set_y(v)
                      ).pack(side=tk.LEFT, padx=1)

        ttk.Separator(right, orient="horizontal").pack(fill=tk.X, pady=6)

        self.asset_label_var = tk.StringVar(value="Texture")
        tk.Label(right, textvariable=self.asset_label_var,
                 font=("Arial", 9), anchor="w").pack(anchor="w")
        self.asset_combo = ttk.Combobox(right)
        self.asset_combo.pack(fill=tk.X)
        self.asset_combo.bind("<<ComboboxSelected>>", self._combo_edit)
        self.asset_combo.bind("<KeyRelease>",         self._combo_edit)

    # ------------------------------------------------------------------ #
    #  COORDINATES                                                         #
    # ------------------------------------------------------------------ #

    def _w2s(self, wx, wz):
        cw = self.canvas.winfo_width()  or CANVAS_WIDTH
        ch = self.canvas.winfo_height() or CANVAS_HEIGHT
        return ((wx - self.cam_x) * self.zoom + cw / 2,
                (wz - self.cam_z) * self.zoom + ch / 2)

    def _s2w(self, sx, sz):
        cw = self.canvas.winfo_width()  or CANVAS_WIDTH
        ch = self.canvas.winfo_height() or CANVAS_HEIGHT
        return ((sx - cw / 2) / self.zoom + self.cam_x,
                (sz - ch / 2) / self.zoom + self.cam_z)

    # ------------------------------------------------------------------ #
    #  DRAWING                                                             #
    # ------------------------------------------------------------------ #

    def draw_canvas(self):
        self.canvas.delete("all")
        cw = self.canvas.winfo_width()  or CANVAS_WIDTH
        ch = self.canvas.winfo_height() or CANVAS_HEIGHT

        for i in range(-200, 200, 10):
            cx, _ = self._w2s(i, 0);  _, cz = self._w2s(0, i)
            self.canvas.create_line(cx, 0, cx, ch, fill="#222222")
            self.canvas.create_line(0, cz, cw,  cz, fill="#222222")
        cx, _ = self._w2s(0, 0);  _, cz = self._w2s(0, 0)
        self.canvas.create_line(cx, 0, cx, ch, fill="#333333", dash=(4, 4))
        self.canvas.create_line(0, cz, cw,  cz, fill="#333333", dash=(4, 4))

        for orig_i, ent in sorted(enumerate(self.entities),
                                   key=lambda t: t[1].get("y", 0)):
            self._draw_entity(ent, orig_i, orig_i == self.selected_index)

        self.canvas.create_text(8, 8, anchor="nw",
                                text=f"zoom x{self.zoom}  |  RMB=pan  scroll=zoom  |  Ctrl+D=Duplicate",
                                fill="#444", font=("Courier", 8))
        self._refresh_listbox()
        self.draw_preview()

    def _draw_entity(self, ent, idx, selected):
        outline = "#ffffff" if selected else "#000000"
        lw      = 3        if selected else 1

        if ent["type"] in ("block", "door"):
            color = self._block_color(ent)
            hx = float(ent.get("sx", 1)) / 2.0
            hz = float(ent.get("sz", 1)) / 2.0
            ex = float(ent["x"]); ez = float(ent["z"])
            x1, z1 = self._w2s(ex - hx, ez - hz)
            x2, z2 = self._w2s(ex + hx, ez + hz)
            
            if ent.get("tile", 1) == 0:
                self.canvas.create_rectangle(x1, z1, x2, z2,
                                             fill=color, outline=outline, width=lw, dash=(4,4))
            else:
                self.canvas.create_rectangle(x1, z1, x2, z2,
                                             fill=color, outline=outline, width=lw)

            if self.zoom >= 7:
                lx, lz = self._w2s(ex, ez)
                self.canvas.create_text(lx, lz,
                    text=os.path.basename(ent.get("texture/enemy", "")),
                    fill="#dddddd", font=("Arial", max(7, int(self.zoom * 0.65))))

            bx, bz = self._w2s(ex - hx + 0.15, ez - hz + 0.15)
            self.canvas.create_text(bx, bz, anchor="nw",
                text=f"y={ent.get('y', 0)}",
                fill="#cccccc" if selected else "#555555",
                font=("Courier", 7))

            if selected:
                for chx, chz in [(ex-hx, ez-hz), (ex+hx, ez-hz),
                                  (ex-hx, ez+hz), (ex+hx, ez+hz)]:
                    px, pz = self._w2s(chx, chz)
                    self.canvas.create_rectangle(
                        px - HANDLE_PX, pz - HANDLE_PX,
                        px + HANDLE_PX, pz + HANDLE_PX,
                        fill="#ffffff", outline="#000000", width=1)

        elif ent["type"] in ("enemy", "pickup"):
            r = max(5, int(self.zoom * 0.65))
            ex, ez = float(ent["x"]), float(ent["z"])
            px, pz = self._w2s(ex, ez)
            
            fill_c = "#cc0000" if ent["type"] == "enemy" else "#00cc44"
            txt_c  = "#ff6666" if ent["type"] == "enemy" else "#66ff66"
            
            self.canvas.create_oval(px-r, pz-r, px+r, pz+r,
                                    fill=fill_c, outline=outline, width=lw)
            if self.zoom >= 7:
                self.canvas.create_text(px, pz + r + 9,
                    text=os.path.basename(ent.get("texture/enemy", "")),
                    fill=txt_c, font=("Arial", max(7, int(self.zoom * 0.6))))

    def _block_color(self, ent):
        if ent["type"] == "door": return "#b8880f"
        t = ent.get("texture/enemy", "").lower()
        if "brick"   in t: return "#7a2020"
        if "wood"    in t: return "#6b3510"
        if "stone"   in t: return "#606060"
        if "tile"    in t: return "#006060"
        if "door"    in t: return "#b8880f"
        if "cursed"  in t: return "#6a1fa8"
        if "scenery" in t: return "#1a6a1a"
        return "#404040"

    # ------------------------------------------------------------------ #
    #  3D MINI PREVIEW                                                     #
    # ------------------------------------------------------------------ #

    def _rotate_preview(self, event):
        if not hasattr(self, "last_prev_x"):
            self.last_prev_x = event.x
            self.last_prev_y = event.y
            return
            
        dx = event.x - self.last_prev_x
        dy = event.y - self.last_prev_y
        
        self.preview_rot_y -= dx * 0.02
        self.preview_rot_x -= dy * 0.02
        
        self.last_prev_x = event.x
        self.last_prev_y = event.y
        self.draw_preview()

    def _on_preview_release(self, event):
        if hasattr(self, "last_prev_x"):
            del self.last_prev_x

    def draw_preview(self):
        self.preview_canvas.delete("all")
        cx, cy = 100, 100 
        scale = max(2.0, self.zoom * 0.5) 
        
        cos_y, sin_y = math.cos(self.preview_rot_y), math.sin(self.preview_rot_y)
        cos_x, sin_x = math.cos(self.preview_rot_x), math.sin(self.preview_rot_x)
        
        def project(px, py, pz):
            px -= self.cam_x
            pz -= self.cam_z
            
            rx = px * cos_y - pz * sin_y
            rz = px * sin_y + pz * cos_y
            
            ry = py * cos_x - rz * sin_x
            
            return cx + rx * scale, cy - ry * scale

        for i, ent in enumerate(self.entities):
            color = "yellow" if i == self.selected_index else "white"
            
            if ent["type"] in ("block", "door"):
                x, y, z = ent["x"], ent["y"], ent["z"]
                sx, sy, sz = ent["sx"] / 2, ent["sy"] / 2, ent["sz"] / 2
                
                corners = [
                    (x-sx, y-sy, z-sz), (x+sx, y-sy, z-sz), (x+sx, y-sy, z+sz), (x-sx, y-sy, z+sz),
                    (x-sx, y+sy, z-sz), (x+sx, y+sy, z-sz), (x+sx, y+sy, z+sz), (x-sx, y+sy, z+sz)
                ]
                
                proj = [project(px, py, pz) for px, py, pz in corners]
                
                edges = [(0,1), (1,2), (2,3), (3,0), (4,5), (5,6), (6,7), (7,4), (0,4), (1,5), (2,6), (3,7)]
                for p1, p2 in edges:
                    self.preview_canvas.create_line(proj[p1][0], proj[p1][1], proj[p2][0], proj[p2][1], fill=color)
                    
            elif ent["type"] in ("enemy", "pickup"):
                px, py = project(ent["x"], ent["y"], ent["z"])
                fc = "red" if ent["type"] == "enemy" else "green"
                self.preview_canvas.create_oval(px-3, py-3, px+3, py+3, outline=fc, fill=fc)

    # ------------------------------------------------------------------ #
    #  ENTITY OPERATIONS                                                   #
    # ------------------------------------------------------------------ #

    def _add_block(self, x=0, y=0, z=0, sx=1, sy=4, sz=1):
        tex = self.available_textures[0] if self.available_textures else "Stone/Stone01.png"
        self.entities.append({
            "type": "block",
            "x": float(round(x)), "y": float(y), "z": float(round(z)),
            "sx": float(sx), "sy": float(sy), "sz": float(sz),
            "texture/enemy": tex,
            "tile": 1 
        })
        self.selected_index = len(self.entities) - 1
        self.populate_properties()
        self.draw_canvas()
        
    def _add_door(self):
        tex = self.available_textures[0] if self.available_textures else "Wood/Wood01.png"
        self.entities.append({
            "type": "door",
            "x": float(round(self.cam_x)), "y": 0.0, "z": float(round(self.cam_z)),
            "sx": 1.0, "sy": 4.0, "sz": 1.0,
            "req_key": "none",
            "texture/enemy": tex,
            "tile": 1
        })
        self.selected_index = len(self.entities) - 1
        self.populate_properties()
        self.draw_canvas()

    def _add_pickup(self):
        tex = self.available_textures[0] if self.available_textures else "Item/Key.png"
        self.entities.append({
            "type": "pickup",
            "x": float(round(self.cam_x)), "y": 0.0, "z": float(round(self.cam_z)),
            "sx": "-", "sy": "-", "sz": "-",
            "pickup_type": "KEY",
            "item_id": "red_key",
            "texture/enemy": tex
        })
        self.selected_index = len(self.entities) - 1
        self.populate_properties()
        self.draw_canvas()

    def _add_enemy(self):
        default_mob = self.available_models[0] if self.available_models else "mobs/enemy01.png"
        self.entities.append({
            "type": "enemy",
            "x": float(round(self.cam_x)), "y": 0.0, "z": float(round(self.cam_z)),
            "sx": "-", "sy": "-", "sz": "-",
            "texture/enemy": default_mob
        })
        self.selected_index = len(self.entities) - 1
        self.populate_properties()
        self.draw_canvas()

    def _delete(self):
        if self.selected_index >= 0:
            del self.entities[self.selected_index]
            self.selected_index = -1
            self.populate_properties()
            self.draw_canvas()

    def duplicate_object(self, event=None):
        if self.selected_index >= 0:
            new_ent = copy.deepcopy(self.entities[self.selected_index])
            
            new_ent["x"] += 1.0 
            new_ent["z"] += 1.0
            
            self.entities.append(new_ent)
            self.selected_index = len(self.entities) - 1
            
            self.populate_properties()
            self.draw_canvas()

    # ------------------------------------------------------------------ #
    #  PROPERTIES PANEL                                                    #
    # ------------------------------------------------------------------ #

    def populate_properties(self):
        for var in self.prop_vars.values():
            var.set("")
        self.asset_combo.set("")

        if not (0 <= self.selected_index < len(self.entities)):
            self.tile_cb.config(state="disabled")
            return

        ent = self.entities[self.selected_index]
        for field, var in self.prop_vars.items():
            var.set(str(ent.get(field, "")))

        if ent["type"] in ("block", "door"):
            self.asset_combo["values"] = self.available_textures
            self.asset_label_var.set("Texture")
            self.tile_cb.config(state="normal")
            self.tile_var.set(ent.get("tile", 1))
        else:
            if ent["type"] == "enemy":
                self.asset_combo["values"] = self.available_models
                self.asset_label_var.set("Texture")
            else:
                self.asset_combo["values"] = self.available_textures
                self.asset_label_var.set("Texture")
            self.tile_cb.config(state="disabled")
            self.tile_var.set(0)
            
        self.asset_combo.set(ent.get("texture/enemy", ""))

    def _prop_edit(self, event=None):
        if not (0 <= self.selected_index < len(self.entities)):
            return
        ent = self.entities[self.selected_index]
        for field, var in self.prop_vars.items():
            if field == "type":
                continue
            val = var.get().strip()
            try:
                if field in ("x","y","z","sx","sy","sz") and val not in ("", "-"):
                    ent[field] = float(val)
                else:
                    ent[field] = val
            except ValueError:
                pass
        self.draw_canvas()

    def _combo_edit(self, event=None):
        if 0 <= self.selected_index < len(self.entities):
            self.entities[self.selected_index]["texture/enemy"] = self.asset_combo.get()
            self.draw_canvas()

    def _tile_edit(self):
        if 0 <= self.selected_index < len(self.entities):
            self.entities[self.selected_index]["tile"] = self.tile_var.get()
            self.draw_canvas()

    def _nudge(self, field, delta):
        if not (0 <= self.selected_index < len(self.entities)):
            return
        ent = self.entities[self.selected_index]
        if str(ent.get(field, "-")) == "-":
            return
        try:
            ent[field] = max(0.5, float(ent[field]) + delta)
            self.prop_vars[field].set(str(ent[field]))
            self.draw_canvas()
        except (ValueError, KeyError):
            pass

    def _set_y(self, yval):
        if not (0 <= self.selected_index < len(self.entities)):
            return
        self.entities[self.selected_index]["y"] = float(yval)
        self.prop_vars["y"].set(str(float(yval)))
        self.draw_canvas()

    def _refresh_listbox(self):
        self.listbox.delete(0, tk.END)
        for i, ent in enumerate(self.entities):
            base = os.path.splitext(os.path.basename(ent["texture/enemy"]))[0]
            if ent["type"] in ("block", "door"):
                parts = ent["texture/enemy"].replace("\\", "/").split("/")
                cat   = parts[0] if len(parts) > 1 else ""
                label = f"[{i}] {ent['type'].capitalize()}  y={ent.get('y',0)}  [{cat}] {base}"
            else:
                label = f"[{i}] {ent['type'].capitalize()}  {base}"
            self.listbox.insert(tk.END, label)
            if i == self.selected_index:
                self.listbox.selection_set(i)

    def _listbox_select(self, event):
        sel = self.listbox.curselection()
        if sel:
            self.selected_index = sel[0]
            self.populate_properties()
            self.draw_canvas()

    # ------------------------------------------------------------------ #
    #  CANVAS EVENTS                                                       #
    # ------------------------------------------------------------------ #

    def _hit_corner(self, sx, sz):
        if not (0 <= self.selected_index < len(self.entities)):
            return None
        ent = self.entities[self.selected_index]
        if ent["type"] not in ("block", "door"):
            return None
        ex = float(ent["x"]); ez = float(ent["z"])
        hx = float(ent["sx"]) / 2; hz = float(ent["sz"]) / 2

        corners = [
            ((ex-hx, ez-hz), (ex+hx, ez+hz)),
            ((ex+hx, ez-hz), (ex-hx, ez+hz)),
            ((ex-hx, ez+hz), (ex+hx, ez-hz)),
            ((ex+hx, ez+hz), (ex-hx, ez-hz)),
        ]
        for (cwx, cwz), (awx, awz) in corners:
            px, pz = self._w2s(cwx, cwz)
            if abs(sx - px) <= HANDLE_PX and abs(sz - pz) <= HANDLE_PX:
                return (awx, awz)
        return None

    def _click(self, event):
        wx, wz = self._s2w(event.x, event.y)

        anchor = self._hit_corner(event.x, event.y)
        if anchor is not None:
            self.drag["mode"]      = "resize"
            self.drag["anchor_wx"] = anchor[0]
            self.drag["anchor_wz"] = anchor[1]
            return

        hit = -1
        for i in range(len(self.entities) - 1, -1, -1):
            ent = self.entities[i]
            if ent["type"] in ("block", "door"):
                hx = float(ent["sx"]) / 2
                hz = float(ent["sz"]) / 2
                if (float(ent["x"]) - hx <= wx <= float(ent["x"]) + hx and
                        float(ent["z"]) - hz <= wz <= float(ent["z"]) + hz):
                    hit = i; break
            elif ent["type"] in ("enemy", "pickup"):
                if math.hypot(float(ent["x"]) - wx, float(ent["z"]) - wz) < 1.0:
                    hit = i; break

        if hit >= 0:
            self.selected_index = hit
            self.drag["mode"]   = "move"
            
            ent = self.entities[hit]
            self.drag["offset_x"] = float(ent["x"]) - wx
            self.drag["offset_z"] = float(ent["z"]) - wz
            
        else:
            self.selected_index = -1
            self.drag["mode"]   = None
            self.drag["offset_x"] = 0
            self.drag["offset_z"] = 0

        self.populate_properties()
        self.draw_canvas()

    def _drag_move(self, event):
        wx, wz = self._s2w(event.x, event.y)

        if self.drag["mode"] == "move" and self.selected_index >= 0:
            new_x = wx + self.drag.get("offset_x", 0)
            new_z = wz + self.drag.get("offset_z", 0)
            
            self.entities[self.selected_index]["x"] = round(new_x * 2) / 2
            self.entities[self.selected_index]["z"] = round(new_z * 2) / 2
            
            self.populate_properties()
            self.draw_canvas()

        elif self.drag["mode"] == "resize" and self.selected_index >= 0:
            ent    = self.entities[self.selected_index]
            aw, az = self.drag["anchor_wx"], self.drag["anchor_wz"]
            new_sx = max(0.5, abs(wx - aw))
            new_sz = max(0.5, abs(wz - az))
            ent["sx"] = round(new_sx * 2) / 2
            ent["sz"] = round(new_sz * 2) / 2
            ent["x"]  = round(((wx + aw) / 2) * 2) / 2
            ent["z"]  = round(((wz + az) / 2) * 2) / 2
            self.populate_properties()
            self.draw_canvas()

    def _drop(self, event):
        self.drag["mode"] = None

    def _pan_start(self, event):
        self.drag["pan_sx"] = event.x
        self.drag["pan_sz"] = event.y

    def _pan_drag(self, event):
        dx = (event.x - self.drag["pan_sx"]) / self.zoom
        dz = (event.y - self.drag["pan_sz"]) / self.zoom
        self.cam_x -= dx; self.cam_z -= dz
        self.drag["pan_sx"] = event.x
        self.drag["pan_sz"] = event.y
        self.draw_canvas()

    def _hover(self, event):
        wx, wz = self._s2w(event.x, event.y)
        sel = f"  sel:[{self.selected_index}]" if self.selected_index >= 0 else ""
        self._status(f"({wx:.1f}, {wz:.1f})  entities:{len(self.entities)}{sel}")

    def _wheel(self, event):
        if event.num == 4 or getattr(event, "delta", 0) > 0:
            self.zoom = min(self.zoom + 1, 50)
        else:
            self.zoom = max(self.zoom - 1, 2)
        self.draw_canvas()

    def _status(self, msg):
        self.status_var.set(msg)

    # ------------------------------------------------------------------ #
    #  SAVE / LOAD                                                         #
    # ------------------------------------------------------------------ #

    def save_map(self):
        maps_dir = os.path.join(self.res_dir, "maps")
        os.makedirs(maps_dir, exist_ok=True)
        fp = filedialog.asksaveasfilename(
            defaultextension=".map", filetypes=[("Map Files", "*.map")],
            initialdir=maps_dir)
        if not fp: return

        # Using tabs '\t' to separate tokens safely when texture paths have folders/spaces
        with open(fp, "w") as f:
            f.write("# SNOW DOOM MAP\n")
            f.write("# block\tx\ty\tz\tsx\tsy\tsz\ttexture\t[1=tile, 0=stretch]\n")
            f.write("# enemy\tx\ty\tz\ttexture\n")
            f.write("# door\tx\ty\tz\tsx\tsy\tsz\treq_key\ttexture\t[1=tile, 0=stretch]\n")
            f.write("# pickup\tx\ty\tz\ttype\titem_id\ttexture\n\n")
            for ent in self.entities:
                name = ent["texture/enemy"] 
                
                if ent["type"] == "block":
                    tile_flag = ent.get('tile', 1)
                    f.write(f"block\t{ent['x']}\t{ent['y']}\t{ent['z']}\t"
                            f"{ent['sx']}\t{ent['sy']}\t{ent['sz']}\t{name}\t{tile_flag}\n")
                elif ent["type"] == "door":
                    req = ent.get('req_key', 'none')
                    tile_flag = ent.get('tile', 1)
                    f.write(f"door\t{ent['x']}\t{ent['y']}\t{ent['z']}\t"
                            f"{ent['sx']}\t{ent['sy']}\t{ent['sz']}\t{req}\t{name}\t{tile_flag}\n")
                elif ent["type"] == "enemy":
                    f.write(f"enemy\t{ent['x']}\t{ent['y']}\t{ent['z']}\t{name}\n")
                elif ent["type"] == "pickup":
                    ptype = ent.get('pickup_type', 'KEY')
                    iid = ent.get('item_id', 'none')
                    f.write(f"pickup\t{ent['x']}\t{ent['y']}\t{ent['z']}\t{ptype}\t{iid}\t{name}\n")

        self._status(f"Saved: {os.path.basename(fp)}")
        messagebox.showinfo("Saved", "Map saved!")

    def load_map(self):
        maps_dir = os.path.join(self.res_dir, "maps")
        os.makedirs(maps_dir, exist_ok=True)
        fp = filedialog.askopenfilename(
            filetypes=[("Map Files", "*.map")], initialdir=maps_dir)
        if not fp: return

        self.entities.clear()
        with open(fp, "r") as f:
            for line in f:
                if not line.strip() or line.startswith("#"): continue
                
                # Split specifically by Tab character so folder trees and paths with spaces stay whole
                p = line.strip().split('\t')
                
                if p[0] == "block" and len(p) >= 8:
                    tile_val = int(p[8]) if len(p) >= 9 else 1
                    self.entities.append({
                        "type": "block",
                        "x": float(p[1]), "y": float(p[2]), "z": float(p[3]),
                        "sx": float(p[4]), "sy": float(p[5]), "sz": float(p[6]),
                        "texture/enemy": p[7], "tile": tile_val
                    })
                elif p[0] == "door" and len(p) >= 9:
                    tile_val = int(p[9]) if len(p) >= 10 else 1
                    self.entities.append({
                        "type": "door",
                        "x": float(p[1]), "y": float(p[2]), "z": float(p[3]),
                        "sx": float(p[4]), "sy": float(p[5]), "sz": float(p[6]),
                        "req_key": p[7], "texture/enemy": p[8],
                        "tile": tile_val
                    })
                elif p[0] == "enemy" and len(p) >= 5:
                    self.entities.append({
                        "type": "enemy",
                        "x": float(p[1]), "y": float(p[2]), "z": float(p[3]),
                        "sx": "-", "sy": "-", "sz": "-", "texture/enemy": p[4]
                    })
                elif p[0] == "pickup" and len(p) >= 7:
                    self.entities.append({
                        "type": "pickup",
                        "x": float(p[1]), "y": float(p[2]), "z": float(p[3]),
                        "sx": "-", "sy": "-", "sz": "-",
                        "pickup_type": p[4], "item_id": p[5], "texture/enemy": p[6]
                    })

        self.selected_index = -1
        self.populate_properties()
        self.draw_canvas()
        self._status(f"Loaded: {os.path.basename(fp)}  ({len(self.entities)} entities)")
        messagebox.showinfo("Loaded", "Map loaded!")


if __name__ == "__main__":
    app = SnowEditor()
    app.mainloop()