.class public final synthetic Lcom/racas/black/GameImporter$$ExternalSyntheticLambda4;
.super Ljava/lang/Object;
.source "D8$$SyntheticClass"

# interfaces
.implements Ljava/lang/Runnable;


# instance fields
.field public final synthetic f$0:Landroid/widget/ProgressBar;

.field public final synthetic f$1:Landroid/widget/TextView;

.field public final synthetic f$2:J

.field public final synthetic f$3:J


# direct methods
.method public synthetic constructor <init>(Landroid/widget/ProgressBar;Landroid/widget/TextView;JJ)V
    .locals 0

    .line 0
    invoke-direct {p0}, Ljava/lang/Object;-><init>()V

    iput-object p1, p0, Lcom/racas/black/GameImporter$$ExternalSyntheticLambda4;->f$0:Landroid/widget/ProgressBar;

    iput-object p2, p0, Lcom/racas/black/GameImporter$$ExternalSyntheticLambda4;->f$1:Landroid/widget/TextView;

    iput-wide p3, p0, Lcom/racas/black/GameImporter$$ExternalSyntheticLambda4;->f$2:J

    iput-wide p5, p0, Lcom/racas/black/GameImporter$$ExternalSyntheticLambda4;->f$3:J

    return-void
.end method


# virtual methods
.method public final run()V
    .locals 6

    .line 0
    iget-object v0, p0, Lcom/racas/black/GameImporter$$ExternalSyntheticLambda4;->f$0:Landroid/widget/ProgressBar;

    iget-object v1, p0, Lcom/racas/black/GameImporter$$ExternalSyntheticLambda4;->f$1:Landroid/widget/TextView;

    iget-wide v2, p0, Lcom/racas/black/GameImporter$$ExternalSyntheticLambda4;->f$2:J

    iget-wide v4, p0, Lcom/racas/black/GameImporter$$ExternalSyntheticLambda4;->f$3:J

    invoke-static/range {v0 .. v5}, Lcom/racas/black/GameImporter;->lambda$copy$3(Landroid/widget/ProgressBar;Landroid/widget/TextView;JJ)V

    return-void
.end method
